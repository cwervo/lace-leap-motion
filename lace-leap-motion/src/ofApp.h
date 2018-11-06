#pragma once

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();

        void keyPressed  (int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void exit();

        ofxLeapMotion leap;
        vector <ofxLeapMotionSimpleHand> simpleHands;

        vector <int> fingersFound;
        ofEasyCam cam;

        ofColor mainColor;
        bool bDrawGridBackground{false};

        vector<ofPoint> points;
        ofPolyline leftHandLine;
        ofPolyline rightHandLine;
        bool bDrawBox{false};
        bool bPinchTest{true};

        ofTrueTypeFont barlowFont;
        struct LeapHand {
            // ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
            // ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
            // ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
            // ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
        };
        ofTrueTypeFont font;

        vector<ofPolyline> leftLines{ofPolyline()};
        vector<ofPolyline> rightLines{ofPolyline()};
        bool leftDrawing;
        bool rightDrawing;

        ofxPanel mainPanel;
        // ofxIntSlider pinchDistance;
        ofxSlider<int> pinchDistance;

        ofShader shader;
};
