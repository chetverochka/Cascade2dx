#include "CCDCamera.h"
#include "CCDirector.h"
#include "layers_scenes_transitions_nodes/CCScene.h"
#include "kazmath/GL/matrix.h"
#include "kazmath/kazmath.h"

USING_NS_CC;

NS_CC_BEGIN;

//cocos2d::CCDCamera* cocos2d::CCDCamera::s_visitableCamera = NULL;

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

void CCDCamera::apply()
{
	const float width = _zoomX * getScaleX();
	const float height = _zoomY * getScaleY();

	const CCPoint anchor = getAnchorPoint();

	const float anchorX = anchor.x * width;
	const float anchorY = anchor.y * height;

	const float x = getPositionX();
	const float y = getPositionY();
	const float rotation = getRotation();

	kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLLoadIdentity();

	kmMat4 orthoMatrix;

	kmMat4OrthographicProjection(
		&orthoMatrix,
		-anchorX,
		width - anchorX,
		-anchorY,
		height - anchorY,
		_nearPlane,
		_farPlane
	);

	kmGLMultMatrix(&orthoMatrix);

	kmGLMatrixMode(KM_GL_MODELVIEW);
	kmGLLoadIdentity();

	kmGLTranslatef(-x, -y, 0.0f);
	kmGLRotatef(-rotation, 0.0f, 0.0f, 1.0f);
}

CCPoint CCDCamera::unproject(CCPoint in, CCSize size) {
	const float width = _zoomX * getScaleX();
	const float height = _zoomY * getScaleY();

	const CCPoint anchor = getAnchorPoint();

	const float anchorX = anchor.x * width;
	const float anchorY = anchor.y * height;

	// Screen coords -> NDC [-1; 1]
	float ndcX = in.x / size.width * 2.0f - 1.0f;
	float ndcY = (size.height - in.y) / size.height * 2.0f - 1.0f;

	// NDC -> coords relative to camera
	float localX = (ndcX + 1.0f) * width * 0.5f - anchorX;
	float localY = (ndcY + 1.0f) * height * 0.5f - anchorY;

	// Back cam rotation
	const float rotation = CC_DEGREES_TO_RADIANS(getRotation());

	const float cosR = cosf(rotation);
	const float sinR = sinf(rotation);

	const float worldX = localX * cosR - localY * sinR + getPositionX();
	const float worldY = localX * sinR + localY * cosR + getPositionY();

	return CCPoint(worldX, worldY);
}

CCPoint CCDCamera::unproject(CCPoint in) {
	return unproject(in, CCDirector::sharedDirector()->getWinSize());
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