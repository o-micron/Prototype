import os
import json
import unittest
from jsonschema import validate


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


DIR = os.path.dirname(os.path.realpath(__file__))


def validateScenes():
    schema = {}
    my_json = {}

    with open(os.path.join(DIR, 'SceneSchema.json'), 'r') as myfile:
        data = myfile.read()
        # parse file
        schema = json.loads(data)

    print("SCENES VALIDATION {")
    for scene_file in ['AirTags.json', 'Bistro.json', 'Cloth.json', 'Demo.json', 'Empty.json', 'Game.json', 'Raytracing2.json', 'Stalingrad.json']:
        filepath = os.path.join(DIR, scene_file)
        with open(filepath, 'r') as myfile:
            data = myfile.read()
            my_json = json.loads(data)

        # Validate will raise exception if given json is not
        # what is described in schema.
        try:
            validate(instance=my_json, schema=schema)
            print("\t" + bcolors.OKGREEN +
                  "✓  {}".format(filepath) + bcolors.ENDC)
        except Exception as e:
            print("\t" + bcolors.FAIL + "✖  {} {}".format(filepath,
                  e.__str__()) + bcolors.ENDC)
    print("}\n")


def validateResources():
    schema = {}
    my_json = {}

    with open(os.path.join(DIR, 'ResourcesSchema.json'), 'r') as myfile:
        data = myfile.read()
        # parse file
        schema = json.loads(data)

    print("RESOURCES VALIDATION {")
    for scene_file in ['Resources_Opengl.json', 'Resources_Vulkan.json']:
        filepath = os.path.join(DIR, scene_file)
        with open(filepath, 'r') as myfile:
            data = myfile.read()
            my_json = json.loads(data)

        # Validate will raise exception if given json is not
        # what is described in schema.
        try:
            validate(instance=my_json, schema=schema)
            print("\t" + bcolors.OKGREEN +
                  "✓  {}".format(filepath) + bcolors.ENDC)
        except Exception as e:
            print("\t" + bcolors.FAIL + "✖  {} {}".format(filepath,
                  e.__str__()) + bcolors.ENDC)
    print("}\n")


validateResources()
validateScenes()
