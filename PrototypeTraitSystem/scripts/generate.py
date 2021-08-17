import os
import sys
import json
from jinja2 import Template, Environment, FileSystemLoader


class file_info():

    def __init__(self, path, name, extension):
        self.path = path
        self.name = name
        self.extension = extension

    def __str__(self):
        return "path: {}, name: {}, extension: {}\n".format(self.path, self.name, self.extension)

    def __repr__(self):
        return "path: {}, name: {}, extension: {}\n".format(self.path, self.name, self.extension)


class blueprint_file():

    def __init__(self, input_file_info, output_file_info):
        self.input_file_info = input_file_info
        self.output_file_info = output_file_info

    def __str__(self):
        return "input: {}, output: {}".format(self.input_file_info, self.output_file_info)

    def __repr__(self):
        return "input: {}, output: {}".format(self.input_file_info, self.output_file_info)


class project_info():

    def __init__(self, dir, scripts_dir, include_dir, src_dir, include_files, src_files, blueprint_header, blueprint_src, meta_file):
        self.dir = dir
        self.scripts_dir = scripts_dir
        self.include_dir = include_dir
        self.src_dir = src_dir
        self.include_files = include_files
        self.src_files = src_files
        self.blueprint_header = blueprint_header
        self.blueprint_src = blueprint_src
        self.meta_file = meta_file

    def __str__(self):
        return "dir: {}\nscripts_dir: {}\ninclude_dir: {}\nsrc_dir: {}\ninclude_files: {}\nsrc_files: {}\nblueprint_header: {}\nblueprint_src: {}\nmeta_file: {}\n".format(self.dir, self.scripts_dir, self.include_dir, self.src_dir, self.include_files, self.src_files, self.blueprint_header, self.blueprint_src, self.meta_file)

    def __repr__(self):
        return "dir: {}\nscripts_dir: {}\ninclude_dir: {}\nsrc_dir: {}\ninclude_files: {}\nsrc_files: {}\nblueprint_header: {}\nblueprint_src: {}\nmeta_file: {}\n".format(self.dir, self.scripts_dir, self.include_dir, self.src_dir, self.include_files, self.src_files, self.blueprint_header, self.blueprint_src, self.meta_file)


def implement_templates(info: project_info):
    traits = [{'name': f.name, 'id': i} for i, f in enumerate(info.include_files)]
    # print(traits)
    bps = [info.blueprint_header, info.blueprint_src]
    # print(bps)
    for bp in bps:
        template_loader = FileSystemLoader(searchpath=bp.input_file_info.path)
        env = Environment(loader=template_loader)
        template = env.get_template(bp.input_file_info.name + bp.input_file_info.extension)
        output = template.render(traits=traits)
        output_path = os.path.join(bp.output_file_info.path, bp.output_file_info.name + bp.output_file_info.extension).replace("\\", "/")
        with open(output_path, "w") as text_file:
            text_file.write("{}".format(output))

def format_code(full_output_filepath):
    os.system("clang-format -i -style=file " +
              os.path.join(full_output_filepath, "PrototypeTraitSystem.h"))
    os.system("clang-format -i -style=file " +
              os.path.join(full_output_filepath, '..', '..', 'src', 'PrototypeTraitSystem.cpp'))


def check_files_need_compilation(info: project_info):
    stored_timestamps = []
    meta_file_path = os.path.join(info.meta_file.path, info.meta_file.name + info.meta_file.extension).replace("\\", "/")
    if not os.path.isfile(meta_file_path):
        print("no meta stored yet.")
        return True

    with open(meta_file_path) as json_file:
        stored_timestamps = json.load(json_file)

    if not stored_timestamps:
        print("empty meta file")
        return True

    for f in info.src_files:
        f_path = os.path.join(f.path, f.name + f.extension).replace('\\', '/')
        if os.path.isfile(f_path):
            if f_path not in stored_timestamps:
                print(f_path)
                return True
            timestamp = os.path.getmtime(f_path)
            if timestamp != stored_timestamps[f_path]:
                print("{} needs compilation".format(f_path))
                return True

    for f in info.include_files:
        f_path = os.path.join(f.path, f.name + f.extension).replace('\\', '/')
        if os.path.isfile(f_path):
            if f_path not in stored_timestamps:
                print("{} needs compilation".format(f_path))
                return True
            timestamp = os.path.getmtime(f_path)
            if timestamp != stored_timestamps[f_path]:
                print("{} needs compilation".format(f_path))
                return True

    blueprint_header_file_input = info.blueprint_header.input_file_info
    blueprint_header_path = os.path.join(blueprint_header_file_input.path, blueprint_header_file_input.name + blueprint_header_file_input.extension).replace("\\", "/")
    if os.path.isfile(blueprint_header_path):
        if blueprint_header_path not in stored_timestamps:
            print("{} needs compilation".format(blueprint_header_path))
            return True
        timestamp = os.path.getmtime(blueprint_header_path)
        if timestamp != stored_timestamps[blueprint_header_path]:
            print("{} needs compilation".format(blueprint_header_path))
            return True

    blueprint_src_file_input = info.blueprint_src.input_file_info
    blueprint_src_path = os.path.join(blueprint_src_file_input.path, blueprint_src_file_input.name + blueprint_src_file_input.extension).replace("\\", "/")
    if os.path.isfile(blueprint_src_path):
        if blueprint_src_path not in stored_timestamps:
            print("{} needs compilation".format(blueprint_src_path))
            return True
        timestamp = os.path.getmtime(blueprint_src_path)
        if timestamp != stored_timestamps[blueprint_src_path]:
            print("{} needs compilation".format(blueprint_src_path))
            return True
        
    return False


def update_timestamps(info: project_info):
    data = {}
    for f in info.src_files:
        f_path = os.path.join(f.path, f.name + f.extension).replace('\\', '/')
        timestamp = os.path.getmtime(f_path)
        data[f_path] = timestamp

    for f in info.include_files:
        f_path = os.path.join(f.path, f.name + f.extension).replace('\\', '/')
        timestamp = os.path.getmtime(f_path)
        data[f_path] = timestamp

    blueprint_header_file_input = info.blueprint_header.input_file_info
    blueprint_header_path = os.path.join(blueprint_header_file_input.path, blueprint_header_file_input.name + blueprint_header_file_input.extension).replace("\\", "/")
    timestamp = os.path.getmtime(blueprint_header_path)
    data[blueprint_header_path] = timestamp

    blueprint_src_file_input = info.blueprint_src.input_file_info
    blueprint_src_path = os.path.join(blueprint_src_file_input.path, blueprint_src_file_input.name + blueprint_src_file_input.extension).replace("\\", "/")
    timestamp = os.path.getmtime(blueprint_src_path)
    data[blueprint_src_path] = timestamp

    meta_file_path = os.path.join(info.meta_file.path, info.meta_file.name + info.meta_file.extension).replace("\\", "/") 
    with open(meta_file_path, 'w') as outfile:
        json.dump(data, outfile)


def setup_project_info(dir):
    scripts_dir = os.path.join(dir, 'scripts').replace("\\", "/")
    include_dir = os.path.join(dir, 'include', 'PrototypeTraitSystem').replace("\\", "/")
    src_dir = os.path.join(dir, 'src').replace("\\", "/")
    blueprints_dir = os.path.join(dir, 'blueprints').replace("\\", "/")
    blueprint_header = blueprint_file(input_file_info=file_info(path=blueprints_dir, name='object_blueprint', extension='.h'), output_file_info=file_info(path=include_dir, name='PrototypeTraitSystem', extension='.h'))
    blueprint_src = blueprint_file(input_file_info=file_info(path=blueprints_dir, name='object_blueprint', extension='.cpp'), output_file_info=file_info(path=src_dir, name='PrototypeTraitSystem', extension='.cpp'))
    include_files = [file_info(path=include_dir, name=f[:-2], extension=f[-2:]) for f in os.listdir(include_dir) if os.path.isfile(os.path.join(include_dir, f)) and not f.startswith(blueprint_header.output_file_info.name)]
    src_files = [file_info(path=src_dir, name=f[:-4], extension=f[-4:]) for f in os.listdir(src_dir) if os.path.isfile(os.path.join(src_dir, f)) and not f.startswith(blueprint_header.output_file_info.name)]
    meta_file = file_info(path=scripts_dir, name='meta', extension='.json')
    return project_info(dir=dir, scripts_dir=scripts_dir, include_dir=include_dir, src_dir=src_dir, include_files=include_files, src_files=src_files, blueprint_header=blueprint_header, blueprint_src=blueprint_src, meta_file=meta_file)


if __name__ == "__main__":
    print("\n\n{}\nSTARTING CODEGEN\n{}\n".format('=' * 60, '=' * 60))
    try:
        if(os.path.isdir(sys.argv[-1])):
            dir = sys.argv[-1]
            info = setup_project_info(dir)
            # print(info)
            if check_files_need_compilation(info=info):
                print("Generating module")
                update_timestamps(info=info)
                implement_templates(info=info)
            else:
                print("Using hot loaded module")
            print("generation process was successfull.\n")
        else:
            print("unsuccessfull, not a valid directory.")
    except Exception as e:
        print("unsuccessfull, caught an error.")
        print(e)
        print("\n")
    print("{}\nENDING CODEGEN.\n{}\n\n".format('=' * 60, '=' * 60))
