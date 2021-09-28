/*
 * Copyright 2009-2021 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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

#ifndef XTP_BIND_TOOLS_H_
#define XTP_BIND_TOOLS_H_

#include "votca/tools/property.h"
#include "votca/xtp/qmtool.h"
#include "votca/xtp/toolfactory.h"
#include <string>

using namespace votca;

namespace pyxtp {
/**
 * @brief Construct a new pybind11 module object to invoke a votca-xtp QMTool
 *
 */
void call_tool(const std::string& calculatorName, Index nThreads,
               std::string xmlfile);

class XTPTools {
 public:
  void Initialize(const std::string& calculatorName, Index nThreads,
                  votca::tools::Property prop);

 private:
  std::unique_ptr<xtp::QMTool> tool_;
};
}  // namespace pyxtp

#endif  // XTP_BIND_TOOLS_H_
