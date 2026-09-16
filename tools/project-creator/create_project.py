#!/usr/bin/python3
# create_project.py
# Create cross-platform cocos2d-x project
# Copyright (c) 2012 cocos2d-x.org
# Author: WangZhe

import sys
import os
import json
import shutil

# define global variables
context = {
    "language": "undefined",
    "src_project_name": "undefined",
    "src_package_name": "undefined",
    "dst_project_name": "undefined",
    "dst_package_name": "undefined",
    "src_project_path": "undefined",
    "dst_project_path": "undefined",
    "script_dir": "undefined",
}
platforms_list = []

def dumpUsage():
    print("Usage: create_project.py -project PROJECT_NAME -package PACKAGE_NAME -language PROGRAMMING_LANGUAGE")
    print("Options:")
    print("  -project   PROJECT_NAME          Project name, for example: MyGame")
    print("  -package   PACKAGE_NAME          Package name, for example: com.MyCompany.MyAwesomeGame")
    print("  -language  PROGRAMMING_LANGUAGE  Major programming language you want to use, should be [cpp | lua | javascript]")
    print("")
    print("Sample 1: ./create_project.py -project MyGame -package com.MyCompany.AwesomeGame")
    print("Sample 2: ./create_project.py -project MyGame -package com.MyCompany.AwesomeGame -language javascript")
    print("")

def checkParams(context):
    context["script_dir"] = os.getcwd() + "/"
    global platforms_list

    if len(sys.argv) < 7:
        dumpUsage()
        sys.exit()

    for i in range(1, len(sys.argv)):
        if sys.argv[i] == "-project":
            context["dst_project_name"] = sys.argv[i+1]
            context["dst_project_path"] = os.path.join(os.getcwd(), "../../projects", context["dst_project_name"])
        elif sys.argv[i] == "-package":
            context["dst_package_name"] = sys.argv[i+1]
        elif sys.argv[i] == "-language":
            context["language"] = sys.argv[i+1]

    raise_error = False
    if context["dst_project_name"] == "undefined":
        print("Invalid -project parameter")
        raise_error = True
    if context["dst_package_name"] == "undefined":
        print("Invalid -package parameter")
        raise_error = True
    if context["language"] == "undefined":
        print("Invalid -language parameter")
        raise_error = True
    if raise_error:
        sys.exit()

    if context["language"] == "cpp":
        context["src_project_name"] = "HelloCpp"
        context["src_package_name"] = "org.cocos2dx.hellocpp"
        context["src_project_path"] = os.path.join(os.getcwd(), "../../template/multi-platform-cpp")
        platforms_list = ["android", "win32"]
    elif context["language"] == "lua":
        context["src_project_name"] = "HelloLua"
        context["src_package_name"] = "org.cocos2dx.hellolua"
        context["src_project_path"] = os.path.join(os.getcwd(), "../../template/multi-platform-lua")
        platforms_list = ["android"]
    elif context["language"] == "javascript":
        context["src_project_name"] = "HelloJavascript"
        context["src_package_name"] = "org.cocos2dx.hellojavascript"
        context["src_project_path"] = os.path.join(os.getcwd(), "../../template/multi-platform-js")
        platforms_list = ["android"]

def replaceString(filepath, src_string, dst_string):
    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()
    content = content.replace(src_string, dst_string)
    with open(filepath, "w", encoding="utf-8") as f:
        f.write(content)

def ensure_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

def movePackageJava(src_root, src_pkg, dst_pkg):
    src_pkg_path = os.path.join(src_root, *src_pkg.split('.'))
    dst_pkg_path = os.path.join(src_root, *dst_pkg.split('.'))
    ensure_dir(os.path.dirname(dst_pkg_path))
    if os.path.exists(src_pkg_path):
        shutil.move(src_pkg_path, dst_pkg_path)
        try:
            parts = src_pkg.split('.')
            cur = src_root
            for p in parts:
                cur = os.path.join(cur, p)
                if os.path.isdir(cur) and not os.listdir(cur):
                    os.rmdir(cur)
                else:
                    break
        except:
            pass
    return dst_pkg_path

def replace_package_in_all_java_files(root_path, src_pkg, dst_pkg):
    for dirpath, dirnames, filenames in os.walk(root_path):
        for fn in filenames:
            if fn.endswith(".java") or fn.endswith(".kt") or fn.endswith(".gradle") or fn.endswith(".xml"):
                full = os.path.join(dirpath, fn)
                try:
                    with open(full, "r", encoding="utf-8") as f:
                        content = f.read()
                    if src_pkg in content:
                        new_content = content.replace(src_pkg, dst_pkg)
                        with open(full, "w", encoding="utf-8") as f:
                            f.write(new_content)
                except UnicodeDecodeError:
                    continue

def processPlatformProjects(platform):
    proj_path = os.path.join(context["dst_project_path"], f"proj.{platform}")

    if platform == "win32":
        src = context["src_project_name"]
        dst = context["dst_project_name"]
        for fn in os.listdir(proj_path):
            if src in fn:
                new_name = fn.replace(src, dst)
                os.rename(os.path.join(proj_path, fn), os.path.join(proj_path, new_name))
        for dirpath, dirnames, filenames in os.walk(proj_path):
            for fn in filenames:
                full = os.path.join(dirpath, fn)
                try:
                    with open(full, "r", encoding="utf-8") as f:
                        content = f.read()
                    if src in content:
                        new_content = content.replace(src, dst)
                        with open(full, "w", encoding="utf-8") as f:
                            f.write(new_content)
                except UnicodeDecodeError:
                    continue
        print(f"proj.{platform}\t\t: Done!")
        return

    with open(f"{platform}.json", "r", encoding="utf-8") as f:
        data = json.load(f)

    app_src_path = os.path.join(proj_path, "app", "src")
    for flavor in ["main", "androidTest", "test"]:
        java_path = os.path.join(app_src_path, flavor, "java")
        if os.path.exists(java_path):
            movePackageJava(java_path, context["src_package_name"], context["dst_package_name"])

    replace_package_in_all_java_files(proj_path, context["src_package_name"], context["dst_package_name"])

    for item in data.get("replace_package_name", []):
        dst_path = os.path.join(proj_path, item)
        if os.path.exists(dst_path):
            replaceString(dst_path, context["src_package_name"], context["dst_package_name"])

    for item in data.get("replace_project_name", []):
        dst_path = os.path.join(proj_path, item)
        if os.path.exists(dst_path):
            replaceString(dst_path, context["src_project_name"], context["dst_project_name"])

    print(f"proj.{platform}\t\t: Done!")

checkParams(context)

if os.path.exists(context["dst_project_path"]):
    print(f"Error: {context['dst_project_path']} folder already exists")
    print("Please remove the old project or choose a new PROJECT_NAME in -project parameter")
    sys.exit()
else:
    shutil.copytree(context["src_project_path"], context["dst_project_path"])

for platform in platforms_list:
    processPlatformProjects(platform)

print("New project has been created in this path:", context["dst_project_path"].replace("/tools/project-creator/../..", ""))
print("Have Fun!")
