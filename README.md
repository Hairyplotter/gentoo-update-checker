# Gentoo Update Checker - KDE Plasma System Tray Widget

A lightweight KDE Plasma system tray widget for Gentoo Linux that monitors package updates and manages the portage database.

## Features

- **Automatic Database Sync**: Syncs the portage database every 24 hours
- **Regular Update Checks**: Checks for available package updates every 4 hours
- **System Tray Integration**: Shows update status with color-coded icons in the system tray
- **Desktop Notifications**: Displays notifications when updates are available
- **Manual Controls**: Trigger sync or check operations manually
- **Terminal Integration**: Quick access to Konsole for package management
- **KDE Plasma Native**: Uses Qt6 for seamless integration

## Requirements

### Runtime
- Gentoo Linux with KDE Plasma
- Qt6 (Core, Gui, Widgets)
- `emerge` package manager
- `pkexec` (from kde-misc/polkit-kde-agent-1) for privilege escalation

### Build
- CMake >= 3.20
- C++ compiler with C++20 support
- Qt6 development headers

## Installation

### On Gentoo Linux

1. **Install dependencies**:
   ```bash
   sudo emerge -av dev-qt/qtbase dev-util/cmake
   ```

2. **Clone and build**:
   ```bash
   git clone https://github.com/Hairyplotter/gentoo-update-checker.git
   cd gentoo-update-checker
   mkdir build && cd build
   cmake -DCMAKE_INSTALL_PREFIX=/usr ..
   make
   sudo make install
   ```

3. **Start the application**:
   ```bash
   gentoo-update-checker
   ```

   Or find it in your application menu under "System" → "Gentoo Update Checker"

## Usage

### System Tray Icon States

- **Green**: No updates available
- **Red**: Updates are available
- **Yellow**: Database sync or update check in progress
- **Dark Red**: An error occurred

### Context Menu Options

- **Show Update Info**: Display current update status
- **Sync Database**: Manually trigger portage database sync (`emerge --sync`)
- **Check Updates**: Manually check for available updates (`emerge -quDN @world`)
- **Open Terminal**: Launch Konsole for direct package management
- **About**: View application information
- **Quit**: Exit the application

### Autostart

The application is configured to autostart with your Plasma session. To disable autostart:
```bash
rm ~/.config/autostart/org.gentoo.UpdateChecker.desktop
```

## Privilege Requirements

Database synchronization requires root privileges and uses `pkexec` for secure privilege escalation. Make sure you have:
- `sudo` configured properly, OR
- `polkit-kde-agent` running in your Plasma session

## Architecture

### Components

- **UpdateChecker**: Core logic for managing sync and update check operations
  - Runs `emerge --sync` to update the portage database
  - Runs `emerge -quDN @world` to check for available updates
  - Emits signals for status changes

- **SystemTrayWidget**: UI and system tray integration
  - Displays status in the system tray
  - Manages context menu
  - Shows notifications
  - Handles user interactions

### Update Check Logic

1. **Sync Phase** (every 24 hours):
   - Runs `emerge --sync` with `pkexec` to update the portage database
   - Updates portage tree and metadata

2. **Check Phase** (every 4 hours):
   - Runs `emerge -quDN @world` to query available updates
   - The command lists all packages that would be updated
   - Exit code 1 indicates updates available, 0 indicates none

## Configuration

Update intervals can be customized by modifying constants in `src/updatechecker.h`:

```cpp
static const int SYNC_INTERVAL_HOURS = 24;      // Database sync interval
static const int CHECK_INTERVAL_MINUTES = 240;  // Update check interval (4 hours)
```

Recompile after changes:
```bash
cd build
make
sudo make install
```

## Troubleshooting

### "Failed to start process"

- Ensure `emerge` is installed: `which emerge`
- Verify `pkexec` is available: `which pkexec`
- Check that the polkit authorization agent is running

### No database sync occurring

- Check if polkit is properly configured for your user
- Try running `emerge --sync` manually with `pkexec` to test
- View logs: `journalctl -xe` for polkit errors

### Updates not detected

- Manually run: `emerge -quDN @world`
- Check if your portage tree is current: `emerge --sync`
- Verify world set: `cat /var/lib/portage/world`

## Development

### Building with Debug Output

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./gentoo-update-checker
```

Debug output will appear in the console and system journal.

### Project Structure

```
gentoo-update-checker/
├── CMakeLists.txt              # Build configuration
├── src/
│   ├── main.cpp                # Application entry point
│   ├── updatechecker.h/.cpp    # Update checker logic
│   ├── systemtraywidget.h/.cpp # UI and tray integration
├── org.gentoo.UpdateChecker.desktop  # Desktop entry for autostart
└── README.md                   # This file
```

## License

GPL-3.0+ (GNU General Public License v3 or later)

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Support

For issues, questions, or suggestions, please open an issue on GitHub.
