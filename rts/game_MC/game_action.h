#pragma once

#include "engine/game_action.h"
#include "mc_rule_actor.h"

class RTSMCAction : public RTSAction {
private:
    enum ActionType { STATE9 = 0, SIMPLE = 1, HIT_AND_RUN = 2, CMD_INPUT = 3 };

public:
    RTSMCAction() : _type(CMD_INPUT), _action(-1) { }

    void SetState9(int action) {
        _type = STATE9;
        _action = action;
    }

    void SetSimpleAI() { _type = SIMPLE; }
    void SetHitAndRunAI() { _type = HIT_AND_RUN; }

    void SetUnitCmds(const std::vector<CmdInput> &unit_cmds) {
        _type = CMD_INPUT;
        _unit_cmds = unit_cmds;
    }

    ///////
    bool Send(const GameEnv &env, CmdReceiver &receiver) override {
        // Apply command. 
        MCRuleActor mc_rule_actor;
        mc_rule_actor.SetPlayerId(_player_id);
        mc_rule_actor.SetReceiver(&receiver);

        vector<int> state(NUM_AISTATE, 0);
        if (_type == CMD_INPUT) {
            mc_rule_actor.ActByCmd(env, _unit_cmds, &_state_string, &_cmds);
        } else {
            bool gather_ok = mc_rule_actor.GatherInfo(env, &_state_string, &_cmds);
            if (! gather_ok) return false;

            switch(_type) {
                case STATE9: 
                    state[_action] = 1;
                    break;
                case SIMPLE:
                    // cout << "Choose simpe AI" << endl;
                    mc_rule_actor.GetActSimpleState(&state);
                    break;
                case HIT_AND_RUN:
                    mc_rule_actor.GetActHitAndRunState(&state);
                    break;
                default:
                    throw std::range_error("Invalid type: " + std::to_string(_type));
            }
            mc_rule_actor.ActByState(env, state, &_state_string, &_cmds);
        }

        return RTSAction::Send(env, receiver);
    }

protected:
    ActionType _type;
    int _action;
    std::vector<CmdInput> _unit_cmds;
};
