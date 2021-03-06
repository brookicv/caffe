#include <cmath>
#include <vector>
#include <fstream>

#include "gtest/gtest.h"

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/filler.hpp"
#include "caffe/layers/squeezedet_loss_layer.hpp"

#include "caffe/test/test_caffe_main.hpp"
#include "caffe/test/test_gradient_check_util.hpp"

namespace caffe {

template <typename TypeParam>
class SqueezeDetLossLayerTest : public MultiDeviceTest<TypeParam> {
  typedef typename TypeParam::Dtype Dtype;

 protected:
  SqueezeDetLossLayerTest()
      : blob_bottom_data_(new Blob<Dtype>(1, 20, 7, 7)),
        blob_bottom_label_(new Blob<Dtype>(6, 1, 1, 1)),
        blob_top_loss_(new Blob<Dtype>()) {
    // Fill the data
    FillerParameter filler_param;
    filler_param.set_std(10);
    GaussianFiller<Dtype> filler(filler_param);
    filler.Fill(this->blob_bottom_data_);
    blob_bottom_vec_.push_back(blob_bottom_data_);
    for (int i = 0; i < blob_bottom_label_->count(); ++i) {
      blob_bottom_label_->mutable_cpu_data()[i] = caffe_rng_rand() % 5;
    }
    blob_bottom_vec_.push_back(blob_bottom_label_);
    blob_top_vec_.push_back(blob_top_loss_);
  }
  virtual ~SqueezeDetLossLayerTest() {
    delete blob_bottom_data_;
    delete blob_bottom_label_;
    delete blob_top_loss_;
  }
  Blob<Dtype>* const blob_bottom_data_;
  Blob<Dtype>* const blob_bottom_label_;
  Blob<Dtype>* const blob_top_loss_;
  vector<Blob<Dtype>*> blob_bottom_vec_;
  vector<Blob<Dtype>*> blob_top_vec_;
};

TYPED_TEST_CASE(SqueezeDetLossLayerTest, TestDtypesAndDevices);

TYPED_TEST(SqueezeDetLossLayerTest, TestRead) {
  typedef typename TypeParam::Dtype Dtype;
  LayerParameter param;
  SqueezeDetLossParameter* squeeze_param = param.mutable_squeezedet_param();
  squeeze_param->set_classes(20);
  squeeze_param->set_anchors_per_grid(2);
  squeeze_param->add_anchor_shapes(10);
  squeeze_param->add_anchor_shapes(20);
  squeeze_param->add_anchor_shapes(30);
  squeeze_param->add_anchor_shapes(40);
  squeeze_param->set_pos_conf(0);
  squeeze_param->set_neg_conf(0);
  squeeze_param->set_lambda_bbox(0);
  SqueezeDetLossLayer<Dtype> layer(param);
  layer.SetUp(this->blob_bottom_vec_, this->blob_top_vec_);
  // EXPECT_EQ(this->blob_top_loss_->cpu_data()[0], 0);
}

}  // namespace caffe
