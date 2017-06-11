#pragma once

// bg_* is shared between Game, CGame and UI - this enum selects the appropriate version of the shared functions.
enum class moduleType {
	game,
	cgame,
	ui,
};
