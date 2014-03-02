//
//  ScenarioEditor.cpp
//  ofxBulletEventsExample
//
//  Created by Ovidiu on 30/12/13.
//
//

#include "ScenarioEditor.h"

//--------------------------------------------------------------
void ScenarioEditor::setup(ofxBulletWorldRigid &world, Scenario &scenario){
    
    this->world = &world;
    this->scenario = &scenario;
    
	// enables mouse Pick events //
	this->world->enableGrabbing(-1);
	ofAddListener(world.MOUSE_PICK_EVENT, this, &ScenarioEditor::onMousePick);

}

//--------------------------------------------------------------
void ScenarioEditor::update(){
}

//--------------------------------------------------------------
void ScenarioEditor::draw(){
}

//--------------------------------------------------------------
void ScenarioEditor::onMousePick( ofxBulletMousePickEvent &e ) {
	
//    cout << "ScenarioEditor::onMousePick : selected a body!!!" << endl;
    for(int i = 0; i < scenario->ScenarioObjects.size(); i++) {
		ofxBulletBaseShape *baseShape;
        baseShape = scenario->ScenarioObjects[i]->getBulletBaseShape();
        if(*baseShape == e) {
            ofLogVerbose("EditorVerbose") << "ScenarioEditor::onMousePick : selected a " << scenario->ScenarioObjects[i]->getObjectName() << endl;
			//mousePickPos = e.pickPosWorld;
			break;
		}
	}
	
	//Set new objects if Editor mode is active and Selected Obstacle
	if(bEscenarioEditorMode){
		
		//Create class to save type of objec and Notifie the Event to escenario
		eventObjectScenario newObjectEvent;
		newObjectEvent.posObject = ofVec3f(e.pickPosWorld);
		newObjectEvent.type = SimpleObject::ShapeTypeObstacle;
		ofNotifyEvent(eventObjectScenario::onNewObject, newObjectEvent);
		
	}

}