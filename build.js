const fs = require("fs");
const path = require("path");
const process = require("child_process");
const moment = require("moment");

const root = __dirname;
const src = path.resolve(root, "./src");
const target = path.resolve(root, "./bin");

function compile(src, target) {
  var pa = fs.readdirSync(src);
  pa.forEach((ele, index) => {
    var info = fs.statSync(path.resolve(src, ele));
    if (!info.isDirectory() && ele.endsWith(".cpp")) {
      console.log("build ", ele.substring(0, ele.length - 4));
      process.exec(
        "g++ " +
        "-std=c++20 " +
        path.resolve(src, ele) +
        " -o " +
        path.resolve(target, ele.substring(0, ele.length - 4))
        );
      }
    });
  console.log();
}


compile(src, target);

fs.watch(src, async (event, filename) => {
  console.log(filename, event);
  var promise = new Promise((resolve, reject) => {
    setTimeout(() => resolve(), 1500);
  });
  await promise;
  if (filename.endsWith(".cpp")) {
    console.log("rebuild ", filename.substring(0, filename.length - 4), "\t", moment().format("YYYY-MM-DD HH:mm:ss"));
    process.execSync(
      "g++ " +
        "-std=c++20 " +
        path.resolve(src, filename) +
        " -o " +
        path.resolve(target, filename.substring(0, filename.length - 4))
    );
  }
});
