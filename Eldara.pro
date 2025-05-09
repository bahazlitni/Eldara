QT       += core gui widgets svg

TARGET    = Eldara
TEMPLATE  = app
RC_FILE   = Eldara.rc

# Use C++20 and enable precompiled headers
CONFIG   += c++20 precompile_header
PRECOMPILE_HEADER = src/pch.h

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

# Use ccache to speed up repeated builds
QMAKE_CC    = ccache gcc
QMAKE_CXX   = ccache g++
QMAKE_LINK  = ccache g++

# Unity (jumbo) build: compile every .cpp via a single TU
SOURCES = \
    src/unity_commands.cpp \
    src/unity_core.cpp \
    src/unity_tools.cpp \
    src/unity_ui.cpp

HEADERS += \
    Grid.h \
    Scene.h \
    Simulator.h \
    Window.h \
    commands/AddVariablesCommand.h \
    commands/ChangeVariablesCommand.h \
    commands/EmbedParamCommand.h \
    commands/InsertObjectsCommand.h \
    commands/MergeSelectionCommand.h \
    commands/MovePointsCommand.h \
    commands/RemoveObjectsCommand.h \
    commands/RemoveVariablesCommand.h \
    commands/SplitDipoleCommand.h \
    objects/Alias.h \
    objects/BCControlPoint.h \
    objects/BCPath.h \
    objects/BCPoint.h \
    objects/Battery.h \
    objects/Capacitor.h \
    objects/DCI.h \
    objects/DCV.h \
    objects/Dipole.h \
    objects/Inductor.h \
    objects/Object.h \
    objects/Resistor.h \
    objects/WorldPoint.h \
    tools/MouseTool.h \
    tools/Grabber.h \
    tools/Pen.h \
    tools/Selector.h \
    utils/Geometry.h \
    utils/Selection.h \
    utils/Globals.h \
    widgets/MainPanel.h \
    widgets/custom/CustomDoubleSpinBox.h \
    widgets/custom/CustomSpinBox.h \
    widgets/groups/AliasGroup.h \
    widgets/groups/ColorsGroup.h \
    widgets/groups/DipoleGroup.h \
    widgets/groups/InputGroup.h \
    widgets/groups/ObjectGroup.h \
    widgets/groups/PenGroup.h \
    widgets/inputs/AddressInput.h \
    widgets/inputs/BooleanInput.h \
    widgets/inputs/ColorInput.h \
    widgets/inputs/CoordinateInput.h \
    widgets/inputs/DoubleParameterInput.h \
    widgets/inputs/IDLabel.h \
    widgets/inputs/SimpleIntegerInput.h \
    widgets/tabs/QuickSettingsTab.h \
    widgets/tabs/SelectionTab.h \
    widgets/tabs/VariablesTab.h \
    dialogs/PreferencesDialog.h

# Deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += assets.qrc
