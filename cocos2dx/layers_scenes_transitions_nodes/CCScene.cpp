/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCScene.h"
#include "support/CCPointExtension.h"
#include "CCDirector.h"
#include "kazmath/kazmath.h"
#include "kazmath/GL/matrix.h"

NS_CC_BEGIN

CCScene::CCScene()
{
    m_bIgnoreAnchorPointForPosition = true;
    setAnchorPoint(ccp(0.5f, 0.5f));

    _defaultCamera = NULL;
    _cameraOrderDirty = true;
    _cameras = CCArray::create();
    _cameras->retain();
}

CCScene::~CCScene()
{
    _cameras->release();
}

bool CCScene::init()
{
    bool bRet = false;
     do 
     {
         CCDirector * pDirector;
         CC_BREAK_IF( ! (pDirector = CCDirector::sharedDirector()) );

         CCSize winSize = pDirector->getWinSize();

         this->setContentSize(winSize);

         _defaultCamera = CCDCamera::create();
         _defaultCamera->setPosition(ccp(
                winSize.width / 2,
                winSize.height / 2
         ));
         addChild(_defaultCamera);

         // success
         bRet = true;


     } while (0);
     return bRet;
}

CCScene *CCScene::create()
{
    CCScene *pRet = new CCScene();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

CCArray* CCScene::getCameras() {
    if (_cameraOrderDirty) {
        reorderCameras();
        _cameraOrderDirty = false;
    }
    return _cameras;
}

void CCScene::visit() {
    CCArray* cameras = getCameras();

    for (int i = 0; i < cameras->count(); i++) {
        CCDCamera* camera = static_cast<CCDCamera*>(cameras->objectAtIndex(i));

        if (!camera || !camera->isVisible()) {
            continue;
        }

        kmGLPushMatrix();

        camera->apply();

        CCNode::visit();
        
        kmGLPopMatrix();
    }

    CCDirector::sharedDirector()->setProjection(ccDirectorProjection::kCCDirectorProjectionDefault);
}

void CCScene::reorderCameras() {
    // sort by depth
    for (int i = 0; i < _cameras->count(); i++) {
        for (int j = 0; j < _cameras->count() - 1 - i; j++) {
            if (_cameras->objectAtIndex(j) > _cameras->objectAtIndex(j + 1)) {
                _cameras->exchangeObjectAtIndex(j, j + 1);
            }
        }
    }
}


NS_CC_END
