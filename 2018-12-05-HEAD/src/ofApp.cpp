#include "ofApp.h"

const int defaultFontSize{32};
bool debug{false};
int handNodesSize{5};
int handConnectorsSize{5};

class Fonts {
    public:
        ofTrueTypeFont ASCIIFont;
        ofTrueTypeFont displayFont;

        void setASCIIFont(string fontPath, int size = defaultFontSize) {
            ASCIIFont.load(fontPath, size);
        }
        void setDisplayFont(string fontPath, int size = defaultFontSize) {
            displayFont.load(fontPath, size);
        }
        Fonts() {
        }
        void setup() {
            // Lato: https://fonts.google.com/specimen/Lato
            setASCIIFont("google-fonts/Lato/Lato-Regular.ttf");
            // Poppins: https://fonts.google.com/specimen/Poppins
            setDisplayFont("google-fonts/Poppins/Poppins-Regular.ttf");
        }
};
Fonts fonts;

class Cursors {
    public:
        vector<ofPoint> cursors;
        ofPoint hiddenPosition = ofPoint(-1000, 0);
        // Turn this into a cursor CLASS with a .hide(x = -1000, y = 0) function :)
        ofPoint left() {
            return cursors[0];
        }
        ofPoint right() {
            return cursors[1];
        }
        void setLeft(ofPoint newPoint) {
            cursors[0] = newPoint;
        }
        void setRight(ofPoint newPoint) {
            cursors[1] = newPoint;
        }
        // Ugh make this so I can do cursors.left().hide()
        void hideLeft() {
            cursors[0] = hiddenPosition;
        }
        void hideRight() {
            cursors[1] = hiddenPosition;
        }

        Cursors() {
            // Left
            cursors.push_back(ofPoint(0,0));
            // Right
            cursors.push_back(ofPoint(0,0));
        }
        void draw() {
            ofPushStyle();
            // ------
            ofSetColor(100, 0, 150);
            // Spheres so  cursors look like circle no matter what!
            // Z-indexing here won't matter when you separate the cameras!!!
            // Do that next: 
            ofDrawSphere(left(), 20);
            ofDrawSphere(right(), 20);
            // ------
            ofPopStyle();
        }
};

class OSInfo {
    public:
        Cursors cursors;
        ofFbo OSLayer;
        void allocateOSLayerBuffer(int w, int h) {
            OSLayer.allocate(w, h);
        }

        void setup() {
            allocateOSLayerBuffer(ofGetWidth(), ofGetHeight());
        }

        OSInfo(int w, int h) {
            // Unnecessary b/c of constructor on cursors!
            // cursors.setup();
            allocateOSLayerBuffer(w, h);
        }
        // Empty constructor
        OSInfo() {}

        void draw() {
            ofPushMatrix();
            OSLayer.begin();
            ofClear(0,0,0,255);
            OSLayer.end();

            // Update FBO for OS layer
            OSLayer.begin();
            cursors.draw();
            OSLayer.end();
            ofPopMatrix();
        }

};
OSInfo globalOSInfo;

class ButtonZone {
    public:
        ofTrueTypeFont buttonFont;
        ofRectangle buttonBounds;
        string buttonLabel{""};
        bool shouldActivateButtonFunction{false};

        void render() {
            ofPushMatrix();
            ofDrawRectangle(buttonBounds);
            buttonFont.drawString(buttonLabel, 0, 0);
            ofPopMatrix();
            // throttle this listener??
            // Don't do this in here, instead do it in draw!!!
            shouldActivateButtonFunction = buttonBounds.inside(globalOSInfo.cursors.left()) || buttonBounds.inside(globalOSInfo.cursors.right());
            // if (shouldActivateButtonFunction) {
            //     doButtonStuff();
            // }

        }

        ButtonZone(ofRectangle bounds, string label, ofTrueTypeFont providedFont) {
            buttonBounds = bounds;
            buttonLabel = label;
            buttonFont = providedFont;
        }
};

class PanelObject {
    public:
        vector<ButtonZone> buttonZones;
        string name;
        ofPoint namePosition;

        void setNamePosition(int w, int h) {
            const double partialStringWidth = 0.30 * (defaultFontSize * name.size());
            namePosition = ofPoint(w * 0.5 - partialStringWidth, h);
        }

        void draw() {
            // Pass in an FBO view???
            // render buttons & Leap canvas context!
        }
        PanelObject(string nameString) {
            name = nameString;
        }

};
class InnerPanels {
    public:
        int currentPanelIndex{0};
        vector<PanelObject> panels;
        PanelObject getCurrentPanel() {
            return panels[currentPanelIndex];
        }

        void previous() {
            const int nextIndex = (currentPanelIndex - 1);
            currentPanelIndex =  nextIndex < 0 ? (panels.size() - 1) : nextIndex;
        }
        void next() {
            currentPanelIndex = (currentPanelIndex + 1) % panels.size();
        }

        void setAllNamePositions() {
            for (int i = 0; i < panels.size(); ++i) {
                panels[i].setNamePosition(ofGetWidth(), 60);
            }
        }
};

InnerPanels innerPanels;

void ofApp::constructInnerPanels() {
    // Info Screen, just render a couple text boxes!
    innerPanels.panels.push_back(PanelObject("About"));
    // Paint
    innerPanels.panels.push_back(PanelObject("Paint"));
    // Circle Toy
    innerPanels.panels.push_back(PanelObject("Circle Toy"));
    // Make a button listener to progress to next panel!!!

    innerPanels.setAllNamePositions();
}

void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);

    // No need, made an explicit constructor in the class.
    globalOSInfo.setup();
    fonts.setup();
    leap.open();
    constructInnerPanels();

    // keep app receiving data from leap motion even when it's in the background
    leap.setReceiveBackgroundFrames(true);

    cam.setOrientation(ofPoint(-20, 0, 0));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
}


//--------------------------------------------------------------
void ofApp::update(){
    // Mock left with mouse!

    fingersFound.clear();

    //here is a simple LACE-2018-12-05 of getting the hands and drawing each finger and joint
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
    ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);

    if (debug) {
        ofSetColor(200);
        ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(leap.isConnected()), 20, 20);
    }

    // Scene Layer
    cam.begin();
    ofPushMatrix();
    /*
       90 along X pushes the view to be parpendicular
       That is: -|
       where "-" is the Leap Motion controller
       and  "|" is the computer screen.
       We want: --, because LACE performed at Gray Area (2018-12-05) is projected down.
       */
    //    ofRotate(90, 0, 0, 1);
    ofSetColor(20);
    ofDrawGridPlane(800, 20, false);
    ofPopMatrix();

    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    for(int i = 0; i < simpleHands.size(); i++){
        bool isLeft        = simpleHands[i].isLeft;
        ofPoint handPos    = simpleHands[i].handPos;
        ofPoint handNormal = simpleHands[i].handNormal;

        ofSetColor(0, 0, 255);
        /*
           Could collapse this into overloaded subscript operator
           e.g.
           globalOSInfo.cursors[i] = ofPoint(handPos.x, handPos.y);
           But keeping this explicit for now :)
         * */
        if (isLeft) {
            globalOSInfo.cursors.setLeft(ofPoint(handPos.x, handPos.y));
            // Failed, it's okay, just use Leap Motion coords :)
            // const ofPoint screenPoint = cam.worldToCamera(ofPoint(ofGetMouseX(), ofGetMouseY(), 0));
            // globalOSInfo.cursors.setLeft(screenPoint);
        } else {
            globalOSInfo.cursors.setRight(ofPoint(handPos.x, handPos.y));
        }

        ofDrawSphere(handPos.x, handPos.y, handPos.z, handNodesSize);
        ofSetColor(255, 255, 0);
        ofDrawArrow(handPos, handPos + 100*handNormal);

        for (int f=0; f<5; f++) {
            ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
            ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
            ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
            ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip

            ofSetColor(0, 255, 0);
            ofDrawSphere(mcp.x, mcp.y, mcp.z, handNodesSize);
            ofDrawSphere(pip.x, pip.y, pip.z, handNodesSize);
            ofDrawSphere(dip.x, dip.y, dip.z, handNodesSize);
            ofDrawSphere(tip.x, tip.y, tip.z, handNodesSize);

            ofSetColor(255, 0, 0);
            ofSetLineWidth(handConnectorsSize);
            ofLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
            ofLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
            ofLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
        }
    }

    // OS Layer
    ofPushMatrix();
    // Z-index: 0 - 100
    const int zIncreaseValue = 10;
    // Substract from max z-index value & multiply to normalize
    const double zScale = (100 - zIncreaseValue) * 0.01;
    ofTranslate(0, 0, zIncreaseValue);
    ofScale(1, 1, zScale);

    globalOSInfo.cursors.draw();

    // ofLog() << globalOSInfo.cursors.left().x;
    ofPopMatrix();
    cam.end();

    // Draw scene name
    ofPushStyle();
    ofSetColor(255,255,255);
    PanelObject scene = innerPanels.getCurrentPanel();
    fonts.displayFont.drawString(scene.name, scene.namePosition.x, scene.namePosition.y);

    if (debug) {
        fonts.displayFont.drawString("num hands: " + to_string(simpleHands.size()), scene.namePosition.x, scene.namePosition.y + 50);
    }
    ofPopStyle();


    // Clear cursors so we know when they're no in the scene!!!
    globalOSInfo.cursors.hideLeft();
    globalOSInfo.cursors.hideRight();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        debug != debug;
    } else if (key == OF_KEY_LEFT) {
        // clear screen?
        innerPanels.previous();
    } else if (key == OF_KEY_RIGHT) {
        // clear screen?
        innerPanels.next();
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
    globalOSInfo.allocateOSLayerBuffer(w, h);
    // Update all panel title positions! ....
    innerPanels.setAllNamePositions();
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
