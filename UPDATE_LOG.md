# ENFi32 Project Update Log

## Latest Update: February 13, 2026 🚀

### Major ESPEasy Upstream Integration

#### 📋 **Update Summary**
- **Source:** letscontrolit/ESPEasy (upstream/mega)
- **Update Date:** February 13, 2026
- **New Commits:** 77 commits from upstream
- **Total Changes:** 842 objects, 755KB pushed to yiit repo

#### 🔧 **Major Features Added**

##### **1. SPI Multiple Bus Support**
- **New Feature:** Support for multiple SPI buses on ESP32
- **Affected Plugins:**
  - P039 (Thermosensors)
  - P095 (ILI9341 Display)
  - P096 (eInk Display) 
  - P099 (XPT2046 Touch)
  - P104 (MAX7219 Dotmatrix)
  - P111, P116, P118, P125, P162, P172
- **Configuration:** SPI bus selector added to plugin settings

##### **2. Web UI Improvements**
- **Log Filtering:** New filter functionality for system logs
- **Interfaces Page:** Separated SPI and I2C configuration
- **Device Page JS:** Enhanced JavaScript functionality
- **Performance:** Better auto-scroll and code optimization

##### **3. Platform Updates**
- **ESP-IDF/Arduino SDK:** Updated to latest version
- **ESP32-P4 Support:** Enhanced ESP32-P4 compatibility
- **ESP32S2 USB Console:** Fixed USB console issues
- **Build System:** Various compilation fixes

##### **4. New Features & Fixes**
- **Cyrillic Font Support:** Added to P104 plugin
- **External EEPROM Settings:** New configuration options
- **Domoticz MQTT:** Improved MQTT communication
- **Timing Stats:** Enhanced console output

#### 💾 **Local ENFi32 Customizations Preserved**
- **Static Files:** 18 custom web files maintained
- **UI Components:** Modern React-like UI preserved
- **Custom CSS:** ENFi32 theme and styling
- **Configuration:** Custom plugin sets and WebServer changes

#### 🔄 **Git Process Used**

```bash
# 1. Check upstream updates
git fetch upstream
# Result: 777 new objects available

# 2. Check commits difference 
git log --oneline mega..upstream/mega
# Result: 77 new commits

# 3. Stash local changes
git stash push -m "Local changes before upstream merge"

# 4. Merge upstream
git merge upstream/mega
# Conflict: GitHub workflow files (build.yml, release.yml)

# 5. Resolve conflicts
git add .github/workflows/build.yml .github/workflows/release.yml
git commit -m "Merge upstream/mega - ESPEasy updates with SPI multi-bus and Web UI improvements"

# 6. Restore local changes
git stash pop
# Success: Auto-merged without conflicts

# 7. Commit local customizations
git add .
git commit -m "Update local customizations after upstream merge"

# 8. Merge to main branch
git checkout main
git merge mega
git commit -m "Merge mega: Integrate ESPEasy upstream updates and local customizations"

# 9. Push to yiit repository
git push origin main
git push origin mega
```

#### ⚠️ **Important Notes for Future Updates**

1. **Always use upstream remote:** `git fetch upstream` before updates
2. **Stash local changes:** Preserve customizations during merge
3. **Expect workflow conflicts:** GitHub Actions files often conflict
4. **Test after merge:** Verify build system still works
5. **Preserve static/ and ui/ folders:** Contains ENFi32 custom UI

#### 📊 **Repository Structure**
- **Upstream:** letscontrolit/ESPEasy (main ESPEasy project)
- **Origin:** yiit/ENFi32_07022026 (our customized fork)
- **Branches:** 
  - `main`: Stable release branch
  - `mega`: Active development branch

#### 🛠️ **Custom Files to Monitor**
These files contain ENFi32-specific customizations:
- `src/src/CustomBuild/define_plugin_sets.h`
- `src/src/WebServer/ESPEasy_WebServer.cpp`
- `static/` folder (entire custom UI)
- `ui/` folder (React-like components)
- `src/Custom-sample.h` (deleted in this update)

#### 📅 **Next Update Schedule**
- **Frequency:** Check monthly or when major features announced
- **Priority Updates:** Security fixes, new ESP32 variants, major features
- **Testing:** Always test build after updates

---

### Previous Updates
*(Add future updates above this line)*

---

**Last Updated:** February 13, 2026  
**Responsible:** Assistant (GitHub Copilot)  
**Status:** ✅ Successfully integrated and pushed