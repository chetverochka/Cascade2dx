#pragma once

#include "base_nodes/CCNode.h"

NS_CC_BEGIN;

class CCScene;

enum ccCameraFlag {
	kCameraFlagDefault = 1 ,
	kCameraFlag1 = 1 << 1,
	kCameraFlag2 = 1 << 2,
	kCameraFlag3 = 1 << 3,
	kCameraFlag4 = 1 << 4,
	kCameraFlag5 = 1 << 5,
	kCameraFlag6 = 1 << 6,
	kCameraFlag7 = 1 << 7,
	kCameraFlag8 = 1 << 8,
	kCameraFlag9 = 1 << 9,
};

class CC_DLL CCDCamera : public cocos2d::CCNode {
public:

	static CCDCamera* create(float zoomX, float zoomY, float nearPlane, float farPlane);
	static CCDCamera* create();

	bool initPerspective(float zoomX, float zoomY, float nearPlane, float farPlane);

	void onEnter() override;
	void onExit() override;

	virtual void setPositionZ(float z);
	virtual float getPositionZ() const;

	void setDepth(int depth);
	int getDepth() const { return _depth; }

	void apply();

	void setCameraFlag(ccCameraFlag flag) { _cameraFlag = flag; }
	ccCameraFlag getCameraFlag() const { return _cameraFlag; }

	//static CCDCamera* getVisitableCamera() { return s_visitableCamera; }

	CCPoint unproject(CCPoint in, CCSize size);
	CCPoint unproject(CCPoint in);
private:
	void setScene(CCScene* scene);

	float _zoomX;
	float _zoomY;
	float _nearPlane;
	float _farPlane;
	float _zPosition;
	int _depth;
	CCScene* _scene;
	ccCameraFlag _cameraFlag;

	static CCDCamera* s_visitableCamera;

	friend class CCScene;
};

NS_CC_END;