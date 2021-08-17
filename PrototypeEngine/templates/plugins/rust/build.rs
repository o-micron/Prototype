R"~~#%#~~(
extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    let profile = std::env::var("PROFILE").unwrap();
    match profile.as_str() {
        "debug" => {
            // Tell cargo to invalidate the built crate whenever the wrapper changes
            println!("cargo:rerun-if-changed=wrapper.h");
            // Tell cargo to tell rustc to link the libs
            // shared library.
            println!("cargo:rustc-link-lib=PrototypeInterface");
            println!("cargo:rustc-link-search=native=../../../build/x64-Debug/bin");
            // The bindgen::Builder is the main entry point
            // to bindgen, and lets you build up options for
            // the resulting bindings.
            let bindings = bindgen::Builder::default()
                // The input header we would like to generate
                // bindings for.
                .header("wrapper.h")
                .clang_arg("-x")
                .clang_arg("c++")
                .clang_arg("-I../../../PrototypeInterface/include")
                .clang_arg("-L../../../build/x64-Debug/bin")
                // Tell cargo to invalidate the built crate whenever any of the
                // included header files changed.
                .parse_callbacks(Box::new(bindgen::CargoCallbacks))
                // Finish the builder and generate the bindings.
                .generate()
                // Unwrap the Result and panic on failure.
                .expect("Unable to generate bindings");

            // Write the bindings to the $OUT_DIR/bindings.rs file.
            let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
            bindings
                .write_to_file(out_path.join("bindings.rs"))
                .expect("Couldn't write bindings!");
        }
        "release" => {
            // Tell cargo to invalidate the built crate whenever the wrapper changes
            println!("cargo:rerun-if-changed=wrapper.h");
            // Tell cargo to tell rustc to link the libs
            // shared library.
            println!("cargo:rustc-link-lib=PrototypeInterface");
            println!("cargo:rustc-link-search=native=../../../build/x64-Release/bin");
            // The bindgen::Builder is the main entry point
            // to bindgen, and lets you build up options for
            // the resulting bindings.
            let bindings = bindgen::Builder::default()
                // The input header we would like to generate
                // bindings for.
                .header("wrapper.h")
                .clang_arg("-x")
                .clang_arg("c++")
                .clang_arg("-I../../../PrototypeInterface/include")
                .clang_arg("-L../../../build/x64-Release/bin")
                // Tell cargo to invalidate the built crate whenever any of the
                // included header files changed.
                .parse_callbacks(Box::new(bindgen::CargoCallbacks))
                // Finish the builder and generate the bindings.
                .generate()
                // Unwrap the Result and panic on failure.
                .expect("Unable to generate bindings");


            // Write the bindings to the $OUT_DIR/bindings.rs file.
            let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
            bindings
                .write_to_file(out_path.join("bindings.rs"))
                .expect("Couldn't write bindings!");
        }
        _ => (),
    };
}
)~~#%#~~"