import sys
from Objects.DriveMotor import DriveMotor
from Objects.ArduinoBridge import ArduinoBridge
from Objects.RobotData import RobotData
from BehaviorManager import BehaviorManager
import time

# Create objects
leftMotor = DriveMotor(35, 37)
rightMotor = DriveMotor(36, 38)
arduinoBridge = ArduinoBridge()


#set team color to team 2
arduinoBridge.setTeamColour(5)


RobotData = RobotData(leftMotor, rightMotor, arduinoBridge)
behaviorManager = BehaviorManager(RobotData)

# start behavior
behaviorManager.start()

# wait on exit
raw_input("Press Enter to exit")
arduinoBridge.setTeamColour(RobotData.teamCol)
#time.sleep(30)

#stop all threads
arduinoBridge.stop()
behaviorManager.stop()
sys.exit()




