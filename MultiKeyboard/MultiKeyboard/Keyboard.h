#pragma once
#include <string>
class Keyboard
{
public:
	virtual void tick() = 0;
	virtual unsigned get_down_state(bool is_ui) = 0;
	virtual unsigned get_frame_action() = 0;
	virtual void set_mapping(uint32_t key, uint32_t val, bool is_ui) = 0;
	virtual void clear_mappings(bool is_ui) = 0;
	virtual std::string get_name() = 0;
	
	unsigned id;
};

