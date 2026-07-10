# Building Gentoo Update Checker

## Prerequisites

### On Gentoo Linux

Install the required development packages:

```bash
sudo emerge --ask dev-qt/qtbase dev-util/cmake
```

## Build Steps

1. **Clone the repository**:
   ```bash
   git clone https://github.com/Hairyplotter/gentoo-update-checker.git
   cd gentoo-update-checker
   ```

2. **Create a build directory**:
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**:
   ```bash
   cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release ..
   ```

   Available CMake options:
   - `-DCMAKE_BUILD_TYPE=Release` - Optimized build (default: Release)
   - `-DCMAKE_BUILD_TYPE=Debug` - Debug build with symbols
   - `-DCMAKE_INSTALL_PREFIX=/usr` - Installation prefix (default: /usr/local)

4. **Build**:
   ```bash
   make -j$(nproc)
   ```

5. **Install**:
   ```bash
   sudo make install
   ```

## Running

### From the build directory (without installing):
```bash
./gentoo-update-checker
```

### After installation:
```bash
gentoo-update-checker
```

### Enable autostart:
The desktop file is installed to `/usr/share/applications/autostart/org.gentoo.UpdateChecker.desktop`

Or manually copy it to your user autostart:
```bash
mkdir -p ~/.config/autostart
cp org.gentoo.UpdateChecker.desktop ~/.config/autostart/
```

## Troubleshooting Build Issues

### CMake not found
```bash
sudo emerge dev-util/cmake
```

### Qt6 not found
```bash
sudo emerge dev-qt/qtbase
```

### Compilation errors

Ensure you have a C++20 capable compiler:
```bash
g++ --version  # Should show GCC 10+ or Clang 10+
```

If needed, update your toolchain:
```bash
sudo emerge --ask sys-devel/gcc
```

### Clean build

If you encounter build issues, try a clean build:
```bash
rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

## Development Build

For development with debug symbols:

```bash
mkdir debug_build
cd debug_build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=~/.local ..
make -j$(nproc)
make install
~/.local/bin/gentoo-update-checker
```

## Testing

### Manual testing

1. Start the application:
   ```bash
   gentoo-update-checker
   ```

2. Verify the system tray icon appears

3. Test menu options:
   - Sync Database
   - Check Updates
   - Open Terminal
   - Show Update Info

4. Monitor output:
   ```bash
   journalctl -f | grep gentoo
   ```

### Debug mode

Build and run with debug output:
```bash
cd debug_build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./gentoo-update-checker
```

Output will show in the console.
