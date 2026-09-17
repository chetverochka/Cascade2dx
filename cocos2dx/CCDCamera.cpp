#include "CCDCamera.h"
#include "CCDirector.h"
#include "layers_scenes_transitions_nodes/CCScene.h"
#include "kazmath/GL/matrix.h"
#include "kazmath/kazmath.h"

USING_NS_CC;

NS_CC_BEGIN;

static CCScene* getSceneRecursive(CCNode* nd) {
	if (nd->getParent()) {
		CCScene* parentScene = dynamic_cast<CCScene*>(nd->getParent());
		if (parentScene) {
			return parentScene;
		}

		return getSceneRecursive(nd->getParent());
	}
	else {
		return NULL;
	}
}

CCDCamera* CCDCamera::create(float zoomX, float zoomY, float nearPlane, float farPlane) {
	CCDCamera* ret = new CCDCamera();
	if (ret && ret->initPerspective(zoomX, zoomY, nearPlane, farPlane)) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return NULL;
}

CCDCamera* CCDCamera::create() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	return CCDCamera::create(winSize.width, winSize.height, -1024, 1024);
}

bool CCDCamera::initPerspective(float zoomX, float zoomY, float nearPlane, float farPlane) {
	if (!CCNode::init()) {
		return false;
	}

	_zoomX = zoomX;
	_zoomY = zoomY;
	_nearPlane = nearPlane;
	_farPlane = farPlane;
	_zPosition = (nearPlane + farPlane) / 2;

	_scene = NULL;

	return true;
}

void CCDCamera::onEnter() {
	CCNode::onEnter();

	if (!_scene) {
		CCScene* scene = getSceneRecursive(this);

		if (scene) {
			setScene(scene);
		}
	}
}

void CCDCamera::onExit() {
	CCNode::onExit();
	setScene(NULL);
}

void CCDCamera::setPositionZ(float z) {
	_zPosition = z;
}

float CCDCamera::getPositionZ() const {
	return _zPosition;
}

void CCDCamera::setDepth(int depth) {
	_depth = depth;
	if (_scene) {
		_scene->_cameraOrderDirty = true;
	}
}

void CCDCamera::apply() {
	float width = _zoomX * getScaleX();
	float height = _zoomY * getScaleY();
	float rotation = getRotation();

	float anchorW = getAnchorPoint().x * width;
	float anchorH = getAnchorPoint().y * height;

	float x = getPositionX();
	float y = getPositionY();

	kmMat4 orthoMatrix;
	kmMat4OrthographicProjection(&orthoMatrix, -width/2, width/2, -height/2, height/2, _nearPlane, _farPlane);

	kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLLoadIdentity();

	kmGLMultMatrix(&orthoMatrix);

	kmGLMatrixMode(KM_GL_MODELVIEW);
	kmGLLoadIdentity();

	kmGLRotatef(rotation, 0.f, 0.f, 1.f);
	kmGLTranslatef(-x + anchorW, -y + anchorH, 0.f);
}

void CCDCamera::setScene(CCScene* scene) {
	if (_scene) {
		if (_scene->_cameras->containsObject(this)) {
			_scene->_cameras->removeObject(this);
			_scene->_cameraOrderDirty = true;
		}
	}

	if (scene) {
		if (!scene->_cameras->containsObject(this)) {
			scene->_cameras->addObject(this);
			scene->_cameraOrderDirty = true;
		}
	}

	_scene = scene;
}

NS_CC_END;