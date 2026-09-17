#pragma once

#include "base_nodes/CCNode.h"

NS_CC_BEGIN;

class CCScene;

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
private:
	void setScene(CCScene* scene);

	float _zoomX;
	float _zoomY;
	float _nearPlane;
	float _farPlane;
	float _zPosition;

	int _depth;

	CCScene* _scene;
};

NS_CC_END;