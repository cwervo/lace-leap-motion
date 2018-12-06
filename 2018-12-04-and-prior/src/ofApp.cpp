#include "ofApp.h"
/* #include <math.h> */

//--------------------------------------------------------------
// consts ....
const bool bDrawGridBackground{false};
const bool bDrawBox{false};
const bool bPinchTest{true};
const bool bDrawHands{true};
const bool bDrawShader{false};
const bool debug{true};

void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    // ofSetLogLevel(OF_LOG_VERBOSE);

    leap.open();

    // keep app receiving data from leap motion even when it's in the background
    leap.setReceiveBackgroundFrames(true);

    // TODO: August 25, try changing this orientation????
    //    cam.setOrientation(ofPoint(-20, 0, 0));
    cam.setOrientation(ofPoint(-90, 0, 0));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    mainColor = ofColor::fromHex(0x2EAFAC);
    mainColor.a = 255 * 0.5;

    barlowFont.load("fonts/Barlow.ttf", 120);
    font.load("fonts/VarelaRound-Regular.ttf", 120);

    mainPanel.setup();
    mainPanel.add(pinchDistance.setup("pinch distance", 5000, 0, 10000));
}


//--------------------------------------------------------------
void ofApp::update(){
    shader.load("","shaders/waves.frag");
    fingersFound.clear();

    //here is a simple LACE-2018-12-04-and-prior of getting the hands and drawing each finger and joint
    //the leap data is delivered in a threaded callback - so it can be easier to work with this copied hand data

    //if instead you want to get the data as it comes in then you can inherit ofxLeapMotion and implement the onFrame method.
    //there you can work with the frame data directly.



    //Option 1: Use the simple ofxLeapMotionSimpleHand - this gives you quick access to fingers and palms.


    simpleHands = leap.getSimpleHands();

    if( leap.isFrameNew() && simpleHands.size() ){

        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
        leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);

        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};

        for(int i = 0; i < simpleHands.size(); i++){
            for (int f=0; f<5; f++) {
                int id = simpleHands[i].fingers[ fingerTypes[f] ].id;
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                fingersFound.push_back(id);
            }
        }
    }



    //Option 2: Work with the leap data / sdk directly - gives you access to more properties than the simple approach
    //uncomment code below and comment the code above to use this approach. You can also inhereit ofxLeapMotion and get the data directly via the onFrame callback.
    /*
       vector <Hand> hands = leap.getLeapHands();
       if( leap.isFrameNew() && hands.size() ){

    //leap returns data in mm - lets set a mapping to our world space.
    //you can get back a mapped point by using ofxLeapMotion::getMappedofPoint with the Leap::Vector that tipPosition returns
    leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
    leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
    leap.setMappingZ(-150, 150, -200, 200);

    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};

    for(int i = 0; i < hands.size(); i++){
    for(int j = 0; j < 5; j++){
    ofPoint pt;

    const Finger & finger = hands[i].fingers()[ fingerTypes[j] ];

    //here we convert the Leap point to an ofPoint - with mapping of coordinates
    //if you just want the raw point - use ofxLeapMotion::getofPoint
    pt = leap.getMappedofPoint( finger.tipPosition() );
    pt = leap.getMappedofPoint( finger.jointPosition(finger.JOINT_DIP) );

    fingersFound.push_back(finger.id());
    }
    }
    }
    */

    //IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
    leap.markFrameAsOld();
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (bDrawGridBackground) {
        ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);
    } else {
        ofBackground(0);
    }

    if (debug) {
        ofSetColor(200);
        ofDrawBitmapString("Lace - Leap Motion Prototype :)\n Num vertices: " + ofToString(leftHandLine.getVertices().size()) +"\nLeap Connected? " + ofToString(leap.isConnected()), 20, 20);
    }

    cam.begin();

    if (bDrawGridBackground) {
        ofPushMatrix();
        // ofRotateZDeg(90);
        ofSetColor(20);
        ofDrawGridPlane(800, 20, false);
        ofPopMatrix();
    }

    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};

    ofPoint leftHandPos;
    ofPoint rightHandPos;

    for(int i = 0; i < simpleHands.size(); i++){
        bool isLeft        = simpleHands[i].isLeft;
        ofPoint handPos    = simpleHands[i].handPos;
        ofPoint handNormal = simpleHands[i].handNormal;

        if (isLeft) {
            leftHandPos = handPos;
        } else {
            rightHandPos = handPos;
        }

        if (bDrawHands) {
            ofSetColor(0, 0, 255);
            ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
            ofSetColor(255, 255, 0);
            ofDrawArrow(handPos, handPos + 100*handNormal);

            for (int f=0; f<5; f++) {
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip

                ofSetColor(mainColor);
                // ofDrawSphere(mcp.x, mcp.y, mcp.z, 6);
                // ofDrawSphere(pip.x, pip.y, pip.z, 6);
                // ofDrawSphere(dip.x, dip.y, dip.z, 6);
                // ofDrawSphere(tip.x, tip.y, tip.z, 20 * sin(ofGetFrameNum() * 0.1));
                ofDrawSphere(mcp.x, mcp.y, mcp.z, 5);
                ofDrawSphere(pip.x, pip.y, pip.z, 5);
                ofDrawSphere(dip.x, dip.y, dip.z, 5);
                ofDrawSphere(tip.x, tip.y, tip.z, 5);

                ofSetColor(255, 0, 0);
                ofSetLineWidth(5);

                ofDrawLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
                ofDrawLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
                ofDrawLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
            }

            if (bPinchTest) {
                ofPushStyle();
                ofPoint thumbTip = simpleHands[i].fingers[THUMB].tip;
                ofPoint indexTip = simpleHands[i].fingers[INDEX].tip;
                ofSetColor(ofColor::fromHex(0x2EAFAC));
                ofSetLineWidth(3);
                ofDrawLine(thumbTip, indexTip);
                ofPoint pinchMidpoint = (thumbTip + indexTip) * 0.5;
                ofSetColor(ofColor::white);
                double dist = thumbTip.squareDistance(indexTip);

                if (dist < pinchDistance) {
                    if (isLeft) {
                        leftDrawing = true;
                        leftLines.back().addVertex(pinchMidpoint);
                    } else {
                        rightDrawing = true;
                        rightLines.back().addVertex(pinchMidpoint);
                    }
                } else {
                    if (isLeft) {
                        if (leftDrawing) {
                            leftLines.push_back(ofPolyline());
                            leftDrawing = false;
                        }
                    } else {
                        if (rightDrawing) {
                            rightLines.push_back(ofPolyline());
                            rightDrawing = false;
                        }
                    }
                }

                // barlowFont.drawString(ofToString(dist) + " HI", pinchMidpoint.x, pinchMidpoint.y);
                ofLog() << to_string(dist);
                ofPopStyle();
            }
        }
    }

    ofPoint size = rightHandPos - leftHandPos;

    if (bDrawBox) {
        ofPushStyle();
        ofSetColor(mainColor);
        ofDrawBox(leftHandPos + ofPoint(size.x * 0.5, size.y * 0.5, 0), size.x, size.y, size.z);
        ofPopStyle();
    }

    ofSetLineWidth(3);
    ofPushStyle();
    ofColor c = ofColor::fromHsb(fmod(ofGetFrameNum() * 0.8, 255), 255, 255);
    c.a *= 0.8;
    ofSetColor(c);
    for (auto line : leftLines) { line.draw(); }
    for (auto line : rightLines) { line.draw(); }
    ofPopStyle();

    cam.end();
    if (debug) {
        mainPanel.draw();
    }

    if (bDrawShader) {
        shader.begin();
        shader.setUniform1f("u_time", ofGetElapsedTimef());
        shader.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
        shader.setUniform2f("u_mouse", ofVec2f(ofGetMouseX(), ofGetMouseY()));

        shader.setUniform3f("rightHandPos", rightHandPos);
        shader.setUniform3f("leftHandPos", leftHandPos);

        ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
        shader.end();
    }

    ofSetWindowTitle(to_string(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        leftLines.clear();
        rightLines.clear();
        leftLines.push_back(ofPolyline());
        rightLines.push_back(ofPolyline());
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
