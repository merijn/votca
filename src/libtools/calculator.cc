/*
 * Copyright 2009-2019 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "../../include/votca/tools/calculator.h"

namespace votca {
namespace tools {

void Calculator::UpdateWithDefaults(Property &options, std::string package) {

  // copy options from the object supplied by the Application
  std::string id = Identify();
  Property options_id = options.get("options." + id);

  // add default values if specified in VOTCASHARE
  char *votca_share = getenv("VOTCASHARE");
  if (votca_share == nullptr) {
    throw std::runtime_error("VOTCASHARE not set, cannot open help files.");
  }
  // load the xml description of the calculator (with defaults and test values)
  std::string xmlFile = std::string(getenv("VOTCASHARE")) + std::string("/") +
                        package + std::string("/xml/") + id +
                        std::string(".xml");

  Property defaults_all;
  defaults_all.LoadFromXML(xmlFile);
  Property defaults = defaults_all.get("options." + id);
  // if a value is given override default values
  AddDefaults(options_id, defaults);

  // options.set("options." + defaults.name(), defaults.value());

  options = defaults;

  // output calculator options
  std::string indent("          ");
  Index level = 1;
  votca::tools::PropertyIOManipulator IndentedText(PropertyIOManipulator::TXT,
                                                   level, indent);
  if (Log::verbose()) {
    std::cout << "\n... ... options\n"
              << IndentedText << options << "... ... options\n"
              << std::flush;
  }
}

void Calculator::AddDefaults(Property &p, Property &defaults) {

  // Go through everything that is defined in user option
  for (Property &prop : p) {
    std::string name = prop.path() + "." + prop.name();

    // std::cout << name << " is given in user options" << std::endl;

    // std::cout << defaults.exists(prop.name()) << std::endl;

    if (prop.HasChildren()) {
      AddDefaults(prop, defaults.get(prop.name()));
    } else if (defaults.exists(prop.name())) {

      defaults.set(prop.name(), prop.value());

    } else {

      defaults.add(prop.name(), prop.value());
    }
  }
}

}  // namespace tools
}  // namespace votca
