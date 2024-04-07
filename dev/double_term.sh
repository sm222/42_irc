#!/bin/bash

# Correct usage of 'pwd' to get the current working directory
SCRIPT_PATH="$(pwd)/dev/run_weechat.sh"

# AppleScript to open iTerm, create a new window, split it horizontally, and execute a specific script in each pane
osascript <<END
tell application "iTerm"
    activate
    try
        set newWindow to (create window with default profile)
    on error
        set newWindow to first window
    end try
    tell newWindow
        tell current session of newWindow
            split horizontally with default profile
        end tell
        delay 1 # Adjusted delay to ensure sessions are ready
        # Dynamically determine sessions as session 1 might not always be available immediately
        set allSessions to sessions of current tab of newWindow
        set scriptPath to "$SCRIPT_PATH"
        repeat with aSession in allSessions
            tell aSession
                write text "bash " & scriptPath & " -d"
                delay 0 # Increased delay between commands to ensure smooth execution
            end tell
        end repeat
    end tell
end tell
END
