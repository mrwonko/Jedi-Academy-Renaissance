#!/bin/python

# creates a code blocks project for each test (test_***.cpp)

import os, re

tests = []
for file in os.listdir():
    match = re.match("test_([^\\.]+)\\.cpp", file)
    if match:
        if match.span() == (0, len(file)):
            tests.append(match.group(1))

template = """<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<CodeBlocks_project_file>
	<FileVersion major="1" minor="6" />
	<Project>
		<Option title="test_{name}" />
		<Option pch_mode="2" />
		<Option compiler="gcc" />
		<Build>
			<Target title="Release">
				<Option output="..\\bin\\test_{name}" prefix_auto="1" extension_auto="1" />
				<Option object_output="obj\\" />
				<Option type="1" />
				<Option compiler="gcc" />
				<Compiler>
					<Add option="-O2" />
					<Add option="-DLUABIND_DYNAMIC_LINK" />
				</Compiler>
				<Linker>
					<Add option="-s" />
					<Add library="lua" />
					<Add library="luabind" />
					<Add directory="..\\lib" />
				</Linker>
			</Target>
			<Target title="Release (jar)">
				<Option output="..\\bin\\jar_test_{name}" prefix_auto="1" extension_auto="1" />
				<Option object_output="jar_obj\\" />
				<Option type="1" />
				<Option compiler="gcc" />
				<Compiler>
					<Add option="-O2" />
					<Add option="-DLUABIND_DYNAMIC_LINK" />
					<Add option="-DLUA_BUILD_AS_DLL" />
					<Add directory="..\\..\\..\\Games\\jarenaissance\\dependencies\\lua-5.1.4-jar\\include" />
				</Compiler>
				<Linker>
					<Add option="-s" />
					<Add library="jar_lua" />
					<Add library="jar_luabind" />
					<Add directory="..\\lib" />
					<Add directory="..\\..\\..\\Games\\jarenaissance\\dependencies\\lua-5.1.4-jar\\lib" />
				</Linker>
			</Target>
		</Build>
		<Compiler>
			<Add option="-Wall" />
			<Add directory=".." />
		</Compiler>
		<Linker>
			<Add directory="..\\lib" />
		</Linker>
		<Unit filename="main.cpp" />
		<Unit filename="test.hpp" />
		<Unit filename="test_{name}.cpp" />
		<Extensions>
			<code_completion />
			<envvars />
			<debugger />
			<lib_finder disable_auto="1" />
		</Extensions>
	</Project>
</CodeBlocks_project_file>
"""

for test in tests:
    file = open("test_{}.cbp".format(test), "w")
    file.write(template.format(name=test))
    file.close()
            
