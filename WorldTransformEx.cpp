#include "WorldTransform.h"
#include "Model.h"
#include "MyMath.h"

void WorldTransform::UpdateMatrix(){
	// スケール、回転、平行移動を合成して行列を計算する
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	
	//親があれば親のワールド行列を計算する
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}

	//定数バッファの転送する
	TransferMatrix();
};
