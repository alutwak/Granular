/* \file algorithm.hpp
 * \brief Contains a set of common algorithms for the granular package
 *
 * (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               August 4, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: August 4, 2019
 */

#pragma once

#include <list>

namespace audioelectric {

  /*!\brief Increments the objects in one "active" list, and moves any of the objects that evaluate to false to a second
   * "inactive" list.
   */
  template <typename T>
  void incrementAndRemove(std::list<T>& active, std::list<T>& inactive) {
    auto itr = active.begin();
    while (itr != active.end()) {
      itr->increment();
      auto old_itr = itr;
      itr++;
      if (!*old_itr) {
        inactive.splice(inactive.end(), active, old_itr);
      }
    }
  }

}  // audioelectric
