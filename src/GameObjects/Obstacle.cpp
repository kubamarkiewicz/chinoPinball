//
//  Obstacle.cpp
//  chinoPinball
//
//  Created by Angel on 02/02/14.
//
//

#include "Obstacle.h"

Obstacle::Obstacle(SimpleMission *currentMission) :
    SimpleObject(currentMission)
{
    
}

//---------------------------------
void Obstacle::setup(ofxBulletWorldRigid &world, ofVec3f position, string url, ofVec3f ModelScale){
    type = ShapeTypeObstacle;
    collisionTime = -120;
    ModelPath = url;
    this->position = position;
    rotation = btQuaternion(btVector3(0,1,0), ofDegToRad(-90));
    
    //TODO to try with ofBtGetCylinderCollisionShape, for improve collision detection
    
    // create ofxBullet shape
    body.create(world.world, position, 0); // we set m=0 for kinematic body

    
    // load 3D model
    scale = ModelScale;
	assimpModel.loadModel(url, true);
	assimpModel.setScale(scale.x, scale.y, scale.z);
	assimpModel.setPosition(0, 0, 0);

    //ofEnableSeparateSpecularLight();
    
	//save init values
	initScale = scale;
	
	
    // add 3D meshes to ofxBullet shape
    for(int i = 0; i < assimpModel.getNumMeshes(); i++)
    {
        body.addMesh(assimpModel.getMesh(i), scale, true);
    }

    assimpModel.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
    assimpModel.playAllAnimations();
    body.add();
    
	
    material.setAmbientColor(ofFloatColor(0, 0, 0));
	material.setDiffuseColor(ofFloatColor(150, 0, 150));
	material.setSpecularColor(ofFloatColor(220, 0, 220));
	material.setShininess(40);
    
    
    body.enableKinematic();
    //body.setProperties(1., 0.); // .25 (more restituition means more energy) , .95 ( friction )
    // to add force to the ball on collision set restitution to > 1
	body.setProperties(3, .95); // restitution, friction
	body.setDamping( .25 );
    
    btTransform transform;
    btRigidBody* a_rb = body.getRigidBody();
    a_rb->getMotionState()->getWorldTransform( transform );
    
    // rotate
    //    btQuaternion currentRotation = transform.getRotation();
    //    btQuaternion rotate = btQuaternion(btVector3(0,0,1), ofDegToRad(degrees));
    btQuaternion rotate;
    
    //    rotation.setRotation(btVector3(0,0,1), ofDegToRad(angle));
    rotate.setEuler(ofDegToRad(0), ofDegToRad(90), ofDegToRad(0));
    transform.setRotation(rotate * rotation);
    
    a_rb->getMotionState()->setWorldTransform( transform );
    
    body.activate();
    
}

//--------------------------------------------------------------
void Obstacle::update(bool bEditorMode){

	autoScalingXYZ();

    assimpModel.update();
    assimpModelMesh = assimpModel.getCurrentAnimatedMesh(0); // Animation Player

	//Udpate mesch if there are changes
	// add 3D mashes to ofxBullet shape
    //for(int i = 0; i < assimpModel.getNumMeshes(); i++)
    //{
		//btVector3 myBtScale;
		//myBtScale.setX(scale.x);
		//myBtScale.setY(scale.y);
		//myBtScale.setZ(scale.z);
		
        //body.getRigidBody()->getCollisionShape()->setLocalScaling(myObjectScale);//->m_collisionShape
		//setImplicitShapeDimensions(myBtScale);
		//addMesh(assimpModel.getMesh(i), scale, true);
    //}
	
	body.activate();

}
/*
//--------------------------------------------------------------
void Obstacle::autoScalingXYZ(){
	
}*/

//--------------------------------------------------------------
void Obstacle::autoScalingXYZ(){
	
	btVector3 myObjectScale;
	ofVec3f myOfObjectScale;
	
	
	if (scaleXyz != last_scaleXyz) {
		float diff = scaleXyz - last_scaleXyz;
		last_scaleXyz = scaleXyz;
		
		//Get Scales
		myObjectScale = body.getRigidBody()->getCollisionShape()->getLocalScaling();
		myOfObjectScale = ofVec3f(myObjectScale.x(), myObjectScale.y(), myObjectScale.z());

		//Update sizes values
		myOfObjectScale += ofMap(diff, 0, initScale.z, 0, 0.45); //+= diff;
		scale += ofMap(diff, 0, initScale.z, 0, 0.025);
		last_scale = scale;

		myObjectScale.setX(myOfObjectScale.x);
		myObjectScale.setY(myOfObjectScale.y);
		myObjectScale.setZ(myOfObjectScale.z);		

		//update physyc object
		body.getRigidBody()->getCollisionShape()->setLocalScaling(myObjectScale);
		assimpModel.setScale(scale.x, scale.y, scale.z);
	}

}

//--------------------------------------------------------------
void Obstacle::draw(bool bEditorMode){
	
	//>>??
	int t = ofGetElapsedTimef()*100-collisionTime;
    if(t<highlightTime){
        ofSetHexColor(highlightColor);
    }else{
        if(/*(SimpleMission::MISSION_CALIFICATIONS  == currentMission->GetMissionState()) && */currentMission->isElementHit(GetObjectId())){
            ofSetHexColor(highlightColor);
        }
        else{
        ofSetHexColor(color);
        }
    }
	//<<??
    


	material.begin();
	body.transformGL();
	
    ofPoint scaleModel = assimpModel.getScale();
    ofScale(scaleModel.x,scaleModel.y,scaleModel.z);
    
	//assimpModelMesh.drawWireframe(); //makes slow framerate
	
	//assimpModelMesh.drawFaces();
	
	//ofScale(scaleModel.x,scaleModel.y,scaleModel.z);
	//assimpModel.getMesh(0).drawFaces(); // Normal..
	//ofPushMatrix();
	//ofScale(scaleModel.x,scaleModel.y,scaleModel.z);
	assimpModel.drawFaces(); // Gigante en el caso del AssimpLoader animado // Model Draw
	//ofPopMatrix();
	//assimpModel.getMesh(0).enableTextures();
	//assimpModel.getMesh(0).drawFaces(); // Mesh Draw
	
	
	/* what is the diference between drawing the faces of the model or the mesh????*/
	
	
	
	
    body.restoreTramsformGL();
	material.end();
	
	
}
//-------------------------------------------------------------
ofxBulletBaseShape* Obstacle::getBulletBaseShape(){
    return (ofxBulletBaseShape*)&body;
}

//------------------------------------------------------------
string Obstacle::getObjectName(){
    return "Obstacle";
}

//------------------------------------------------------------
void Obstacle::onCollision(){
    
    GameStatus::getInstance()->AddPoints(1);
    //save time to show color during some time
    collisionTime = ofGetElapsedTimef()*100;
    //play sound
    SoundManager::getInstance()->PlaySound(0);
   
	//Play rele //TODO After try to move this to SimpleObject ... then all objects will
	eventComunication newComEvent;
	newComEvent.collision = true;
    newComEvent.pObject = this;
	ofNotifyEvent(eventComunication::onNewCom, newComEvent);
}

//------------------------------------------------------------
void Obstacle::setDefaultZ(){
    
    position.z = -0.5;
    setPosition(position);
    
}

//------------------------------------------------------------
void Obstacle::setPosition(ofVec3f position){
    
    btTransform transform;
    btRigidBody* rigidBody = body.getRigidBody();
    rigidBody->getMotionState()->getWorldTransform( transform );
    btVector3 origin;
    origin.setX(position.x);
    origin.setY(position.y);
    origin.setZ(position.z);
    transform.setOrigin(origin);
    rigidBody->getMotionState()->setWorldTransform( transform );
    
}


