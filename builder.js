#!/usr/bin/env node
var name = process.argv[2];
var shell = require("shelljs");

const path = require("path");

const root = __dirname;
const src = path.resolve(root, "./src");
const target = path.resolve(root, "./bin");

shell.exec(
    "clang++ " + "-std=c++20 " +
    path.resolve(src, name + ".cpp") + " -o " +
    path.resolve(target, name)
)
