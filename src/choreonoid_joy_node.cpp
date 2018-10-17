#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <cnoid/Joystick>

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "choreonoid_joy");

    cnoid::Joystick joystick;
    if(!joystick.isReady()){
        cerr << joystick.errorMessage() << endl;
        return 1;
    }

    int seq = 0;
    bool stateChanged = false;
    
    joystick.sigButton().connect([&](int, bool){ stateChanged = true; });
    joystick.sigAxis().connect([&](int, double){ stateChanged = true; });
        
    ros::NodeHandle node;
    ros::Publisher publisher = node.advertise<sensor_msgs::Joy>("/joy", 30);

    ros::Rate loop_rate(60);

    while(ros::ok()){
        joystick.readCurrentState();
        if(stateChanged){
            sensor_msgs::Joy joy;
            joy.header.seq = seq++;
            joy.header.stamp = ros::Time::now();
            const int numAxes = joystick.numAxes();
            joy.axes.resize(numAxes);
            for(int i=0; i < numAxes; ++i){
                joy.axes[i] = joystick.getPosition(i);
            }
            const int numButtons = joystick.numButtons();
            joy.buttons.resize(numButtons);
            for(int i=0; i < numButtons; ++i){
                joy.buttons[i] = joystick.getButtonState(i) ? 1 : 0;
            }
            publisher.publish(joy);
            stateChanged = false;
        }
        loop_rate.sleep();
    }
    return 0;
}
