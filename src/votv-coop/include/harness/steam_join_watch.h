// harness/steam_join_watch.h -- a join asked for from outside the game: the Steam lobby bridge.
//
// A friend accepts a Steam invite; the bridge (VotVMP\steam-auto.js, outside this process) joins the
// Steam lobby, opens its local UDP end and writes the address the game should dial into
// `multivoid_join.txt` beside the game exe (one line, "host:port"). This watch picks the file up,
// deletes it and runs the same Direct connect the server browser's box runs, so the player does not
// type 127.0.0.1:47621 by hand. A request while a session already runs is dropped with a log line.
//
// A worker thread polls twice a second; ConnectDirect needs no game thread (it only queues a start).

#pragma once

namespace harness::steam_join_watch {

// Start the watch once (later calls are no-ops). Any thread.
void Start();

}  // namespace harness::steam_join_watch
