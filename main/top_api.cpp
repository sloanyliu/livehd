//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

// top_api. This are commands at the top level like filter, files...

#include "top_api.hpp"

#include <dirent.h>

#include <iostream>
#include <regex>
#include <string>

void Top_api::files(Eprp_var &var) {
  std::string path(var.get("path"));
  std::string match(var.get("match"));
  std::string filter(var.get("filter"));

  try {
    const std::regex txt_regex(match);
    const std::regex filter_regex(filter);

    DIR *dirp = opendir(path.c_str());
    if (dirp == 0) {
      Main_api::error("invalid path:{}, is it a valid directory?", path);
      return;
    }
    std::vector<std::string> sort_files;
    struct dirent *          dp;
    while ((dp = readdir(dirp)) != NULL) {
      if (dp->d_type == DT_DIR)
        continue;
      if (match.empty()) {
        if (filter.empty()) {
          sort_files.push_back(dp->d_name);
        } else if (!std::regex_search(dp->d_name, filter_regex)) {
          sort_files.push_back(dp->d_name);
        }
      } else if (std::regex_search(dp->d_name, txt_regex)) {
        if (filter.empty()) {
          sort_files.push_back(dp->d_name);
        } else if (!std::regex_search(dp->d_name, filter_regex)) {
          sort_files.push_back(dp->d_name);
        }
      }
    }
    closedir(dirp);

    std::sort(sort_files.begin(), sort_files.end());
    std::string files;
    for (const auto &s : sort_files) {
      if (!files.empty())
        files.append(",");
      files.append(path);
      files.append("/");
      files.append(s);
    }

    var.add("files", files);
    var.delete_label("path");  // Path was used for looking for files

  } catch (const std::regex_error &e) {
    Main_api::error(
        "invalid regex. It is a FULL regex unlike bash. To test, try: `ls path | grep -E \"match\" | grep -v \"filter\"`",
        match);
  }
}

void Top_api::setup(Eprp &eprp) {
  // Alphabetical order sorted to avoid undeterminism in different file orders
  Eprp_method m1("files", "match file names in alphabetical order. Like `ls {path} | grep -E {match} | sort`", &Top_api::files);
  m1.add_label_optional("path", "path to match the search . by default", ".");
  m1.add_label_optional("match", "quoted string of regex to match. E.g: match:\"\\.v$\" for verilog files.");
  m1.add_label_optional("filter", "quoted string of regex to filter.");

  eprp.register_method(m1);
}
