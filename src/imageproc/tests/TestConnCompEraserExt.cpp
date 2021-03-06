// Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
// Use of this source code is governed by the GNU GPLv3 license that can be found in the LICENSE file.

#include <BWColor.h>
#include <BinaryImage.h>
#include <ConnComp.h>
#include <ConnCompEraserExt.h>
#include <RasterOp.h>

#include <QImage>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <list>

#include "Utils.h"

namespace imageproc {
namespace tests {
using namespace utils;

BOOST_AUTO_TEST_SUITE(ConnCompEraserExtTestSuite)

BOOST_AUTO_TEST_CASE(test_null_image) {
  ConnCompEraser eraser(BinaryImage(), CONN4);
  BOOST_CHECK(eraser.nextConnComp().isNull());
}

static bool checkAlignedImage(const ConnCompEraserExt& eraser, const BinaryImage& nonaligned) {
  const BinaryImage aligned(eraser.computeConnCompImageAligned());
  const int pad = aligned.width() - nonaligned.width();
  if (pad < 0) {
    return false;
  }

  BinaryImage test1(nonaligned);
  BinaryImage empty1(test1.size());
  empty1.fill(WHITE);
  rasterOp<RopXor<RopSrc, RopDst>>(test1, test1.rect(), aligned, QPoint(pad, 0));
  if (test1 != empty1) {
    return false;
  }

  if (pad > 0) {
    // Check that padding is white.
    BinaryImage test2(pad, nonaligned.height());
    BinaryImage empty2(test2.size());
    empty2.fill(WHITE);
    rasterOp<RopSrc>(test2, test2.rect(), aligned, QPoint(0, 0));
    if (test2 != empty2) {
      return false;
    }
  }
  return true;
}

BOOST_AUTO_TEST_CASE(test_small_image) {
  static const int inp[]
      = {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0,
         0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0};

  std::list<BinaryImage> c4i;

  static const int out4_1[] = {1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0};
  c4i.push_back(makeBinaryImage(out4_1, 3, 6));

  static const int out4_2[] = {1, 1};
  c4i.push_back(makeBinaryImage(out4_2, 2, 1));

  static const int out4_3[] = {0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1};
  c4i.push_back(makeBinaryImage(out4_3, 7, 2));

  static const int out4_4[] = {
      1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0,
  };
  c4i.push_back(makeBinaryImage(out4_4, 4, 3));

  static const int out4_5[] = {1};
  c4i.push_back(makeBinaryImage(out4_5, 1, 1));

  static const int out4_6[] = {1};
  c4i.push_back(makeBinaryImage(out4_6, 1, 1));

  std::list<BinaryImage> c8i;

  static const int out8_1[] = {
      0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1,
      1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  };
  c8i.push_back(makeBinaryImage(out8_1, 9, 6));

  static const int out8_2[] = {
      0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  };
  c8i.push_back(makeBinaryImage(out8_2, 8, 2));

  BinaryImage img(makeBinaryImage(inp, 9, 8));

  ConnComp cc;
  ConnCompEraserExt eraser4(img, CONN4);
  while (!(cc = eraser4.nextConnComp()).isNull()) {
    const BinaryImage ccImg(eraser4.computeConnCompImage());
    const auto it(std::find(c4i.begin(), c4i.end(), ccImg));
    if (it != c4i.end()) {
      BOOST_CHECK(checkAlignedImage(eraser4, ccImg));
      c4i.erase(it);
    } else {
      BOOST_ERROR("Incorrect 4-connected block found.");
    }
  }
  BOOST_CHECK_MESSAGE(c4i.empty(), "Not all 4-connected blocks were found.");

  ConnCompEraserExt eraser8(img, CONN8);
  while (!(cc = eraser8.nextConnComp()).isNull()) {
    const BinaryImage ccImg(eraser8.computeConnCompImage());
    const auto it(std::find(c8i.begin(), c8i.end(), ccImg));
    if (it != c8i.end()) {
      BOOST_CHECK(checkAlignedImage(eraser8, ccImg));
      c8i.erase(it);
    } else {
      BOOST_ERROR("Incorrect 8-connected block found.");
    }
  }
  BOOST_CHECK_MESSAGE(c8i.empty(), "Not all 8-connected blocks were found.");
}

BOOST_AUTO_TEST_SUITE_END()
}  // namespace tests
}  // namespace imageproc