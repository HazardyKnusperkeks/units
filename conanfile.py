from conans import ConanFile, CMake

class UnitConan(ConanFile):
    name = "units"
    version = "0.0.1"
    author = "Mateusz Pusz"
    license = "https://github.com/mpusz/units/blob/master/LICENSE"
    url = "https://github.com/mpusz/units"
    description = "Physical Units library for C++"
    settings = "os", "compiler", "build_type", "arch"

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_dir="%s/src" % self.source_folder)
        cmake.build()
        # cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["units"]
