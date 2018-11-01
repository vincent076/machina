from threading import Thread
import time
import OSC

from BehaviorStates.BaseState import BaseState
from BehaviorStates.IdleState import IdleState
from BehaviorStates.ShakenState import ShakenState
from BehaviorStates.AngeredState import AngeredState
from BehaviorStates.BeingPetState import BeingPetState
from BehaviorStates.ReturnToBaseState import ReturnToBaseState
from BehaviorStates.RemoveSnowState import RemoveSnowState
from BehaviorStates.AttackBaseState import AttackBaseState
from BehaviorStates.DriveState import DriveState

states = {
    "IdleState": IdleState,
    "BaseState": BaseState,
    "ShakenState": ShakenState,
    "AngeredState": AngeredState,
    "BeingPetState": BeingPetState,
    "ReturnToBaseState": ReturnToBaseState,
    "RemoveSnowState": RemoveSnowState,
    "AttackBaseState": AttackBaseState,
    "DriveState": DriveState
}

class BehaviorManager:

    def __init__(self, robotData):
        print "Create behavior manager with default behavior 'IdleState'"

        self.robotData = robotData
        self.lastTime = time.time()
        self.active = False
        self.thread = None

        self.currentState = BaseState()
        self.setState("IdleState")

        # setup server
        receive_address = '192.168.0.255', 8000
        self.server = OSC.ThreadingOSCServer(receive_address) 
        
        # setup handlers
        self.server.addMsgHandler("/IdleState", self.changeStateWithOSC)
        self.server.addMsgHandler("/BaseState", self.changeStateWithOSC)
        self.server.addMsgHandler("/ShakenState", self.changeStateWithOSC)
        self.server.addMsgHandler("/AngeredState", self.changeStateWithOSC)
        self.server.addMsgHandler("/BeingPetState", self.changeStateWithOSC)
        self.server.addMsgHandler("/ReturnToBaseState", self.changeStateWithOSC)
        self.server.addMsgHandler("/RemoveSnowState", self.changeStateWithOSC)
        self.server.addMsgHandler("/AttackBaseState", self.changeStateWithOSC)
        self.server.addMsgHandler("/DriveState", self.changeStateWithOSC)

        print "Serving on {}".format(self.server.server_address) 
        self.server_thread = Thread(target=self.server.serve_forever)
        self.server_thread.start()
        print "done" 

    def start(self):
        """ Start the behavior manager in a new thread """

        self.active = True
        self.thread = Thread(target=self.loop, args=())
        self.thread.start()

    def stop(self):
        """ Stop the behavior thread. This will also stop the current behavior """
        self.active = False

    def changeStateWithOSC(self, addr, tags, stuff, source):
        state_to_enter = addr[1:]
        if state_to_enter in states.keys():
            self.setState(state_to_enter)
        else:
            print "failed on state " + state_to_enter


    def setState(self, new_state_name):
        """ Change the current behavior state """
        if not (self.currentState.stateName == new_state_name):       # Ensuring the same state can't be entered while in that state
            print "left state: " + self.currentState.stateName
            self.currentState.onLeave()
            self.currentState = states[new_state_name]()
            self.currentState.setRobotData(self.robotData)
            self.currentState.onEnter()
            print "enter new state: '" + new_state_name + "'"

    def loop(self):
        while self.active:
            delta = time.time() - self.lastTime
            self.lastTime = time.time()

            if self.currentState.nextBehavior is None:
                self.currentState.onUpdate(delta)
            else:
                if not (self.currentState.stateName == self.currentState.nextBehavior):
                    self.setState(self.currentState.nextBehavior)
                else:
                    self.currentState.onUpdate(delta)

