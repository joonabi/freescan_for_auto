import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"
import "../styles"

Item {
    id: root
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0

    property bool emergencyStop: false

    signal onClientChange(string name, string logo)

    // 데이터 모델 (DataManager에서 로드)
    property var clients: dataManager.loadClients()
    property var locations: {
        var arr = dataManager.loadLocations(); var r = []; for(var i=0;i<arr.length;i++) r.push(arr[i]); return r;
    }
    property var tasks: {
        var arr = dataManager.loadTasks(); var r = []; for(var i=0;i<arr.length;i++) r.push(arr[i]); return r;
    }
    property var workers: {
        var arr = dataManager.loadWorkers(); var r = []; for(var i=0;i<arr.length;i++) r.push(arr[i]); return r;
    }

    // 시스템 설정
    property var sysSettings: dataManager.loadSettings()
    property string selectedClient: sysSettings.selectedClient || "현대자동차"
    property string selectedLocation: sysSettings.selectedLocation || "본사공장"
    property string selectedTask: sysSettings.selectedTask || "외관검사"
    property string selectedWorker: sysSettings.selectedWorker || "김철수"

    property string logoPreview: ""

    property bool showClientManager: false
    property bool showClientInput: false
    property bool showLocationInput: false
    property bool showTaskInput: false
    property bool showWorkerInput: false

    // 설정 저장 함수
    function saveWorkplaceSettings() {
        var s = dataManager.loadSettings()
        s.selectedClient   = selectedClient
        s.selectedLocation = selectedLocation
        s.selectedTask     = selectedTask
        s.selectedWorker   = selectedWorker
        dataManager.saveSettings(s)
    }

    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            width: parent.parent.width
            spacing: 32
            padding: 32

            // 제목
            Text {
                text: "작업장 관리"
                color: Colors.textPrimary
                font.pixelSize: 32
                font.bold: true
            }

            // 2열 그리드
            GridLayout {
                Layout.fillWidth: true
                columns: parent.width > 1200 ? 2 : 1
                columnSpacing: 32
                rowSpacing: 32

                // === 좌측: 수요처 관리 ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 700
                    color: Colors.bgCard
                    radius: 16

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 32
                        spacing: 24

                        Text {
                            text: "수요처 관리"
                            color: Colors.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }

                        // 수요처 선택/관리 헤더
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 16

                            Text {
                                text: "수요처 선택"
                                color: Colors.textSecondary
                                font.pixelSize: 18
                                font.bold: true
                            }

                            Item { Layout.fillWidth: true }

                            Button {
                                text: showClientManager ? "선택 모드" : "관리"
                                enabled: !root.emergencyStop

                                background: Rectangle {
                                    color: parent.enabled ? Colors.btnBlue : Colors.btnGray
                                    radius: 8
                                }

                                contentItem: Text {
                                    text: parent.text
                                    color: Colors.textPrimary
                                    font.pixelSize: 16
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    showClientManager = !showClientManager
                                    showClientInput = false
                                }
                            }
                        }

                        // 드롭다운 모드
                        ComboBox {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 60
                            visible: !showClientManager
                            enabled: !root.emergencyStop

                            model: {
                                var names = []
                                for (var i = 0; i < clients.length; i++)
                                    names.push(clients[i].name)
                                return names
                            }
                            currentIndex: {
                                for (var i = 0; i < clients.length; i++)
                                    if (clients[i].name === selectedClient) return i
                                return 0
                            }

                            onActivated: {
                                selectedClient = clients[currentIndex].name
                            }

                            delegate: ItemDelegate {
                                width: parent.width
                                contentItem: Text {
                                    text: modelData
                                    color: Colors.textPrimary
                                    font.pixelSize: 16
                                    verticalAlignment: Text.AlignVCenter
                                }
                                background: Rectangle {
                                    color: parent.highlighted ? Colors.btnBlue : Colors.bgTertiary
                                }
                            }
                        }

                        // 관리 모드
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            visible: showClientManager
                            color: Colors.bgTertiary
                            radius: 12

                            ScrollView {
                                anchors.fill: parent
                                anchors.margins: 16
                                clip: true

                                ColumnLayout {
                                    width: parent.width
                                    spacing: 12

                                    Repeater {
                                        model: clients

                                        delegate: Rectangle {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 80
                                            color: selectedClient === modelData.name ? Colors.btnBlue : Colors.bgCard
                                            radius: 8

                                            RowLayout {
                                                anchors.fill: parent
                                                anchors.margins: 16
                                                spacing: 12

                                                ColumnLayout {
                                                    Layout.fillWidth: true
                                                    spacing: 4

                                                    Text {
                                                        text: modelData.name
                                                        color: Colors.textPrimary
                                                        font.pixelSize: 18
                                                        font.bold: true
                                                    }

                                                    Text {
                                                        text: modelData.type
                                                        color: Colors.textTertiary
                                                        font.pixelSize: 14
                                                    }
                                                }

                                                Button {
                                                    text: "🗑"
                                                    visible: clients.length > 1

                                                    background: Rectangle {
                                                        color: parent.pressed ? Colors.btnRedHover : Colors.btnRed
                                                        radius: 8
                                                    }

                                                    contentItem: Text {
                                                        text: parent.text
                                                        font.pixelSize: 20
                                                        horizontalAlignment: Text.AlignHCenter
                                                    }

                                                    onClicked: {
                                                        var newClients = []
                                                        for (var i = 0; i < clients.length; i++)
                                                            if (clients[i].id !== modelData.id)
                                                                newClients.push(clients[i])
                                                        clients = newClients
                                                        dataManager.saveClients(clients)
                                                        if (selectedClient === modelData.name && clients.length > 0)
                                                            selectedClient = clients[0].name
                                                    }
                                                }
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                z: -1
                                                onClicked: selectedClient = modelData.name
                                            }
                                        }
                                    }

                                    // 추가 버튼
                                    Button {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 60
                                        text: showClientInput ? "취소" : "+ 추가"

                                        background: Rectangle {
                                            color: showClientInput ? Colors.btnGray : Colors.btnGreen
                                            radius: 8
                                        }

                                        contentItem: Text {
                                            text: parent.text
                                            color: Colors.textPrimary
                                            font.pixelSize: 18
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                        }

                                        onClicked: showClientInput = !showClientInput
                                    }

                                    // 입력 폼
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 180
                                        visible: showClientInput
                                        color: Colors.bgCard
                                        radius: 8

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 16
                                            spacing: 12

                                            TextField {
                                                id: newClientName
                                                Layout.fillWidth: true
                                                placeholderText: "새 수요처 이름"
                                                color: Colors.textPrimary
                                                font.pixelSize: 16

                                                background: Rectangle {
                                                    color: Colors.bgTertiary
                                                    radius: 8
                                                    border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary
                                                    border.width: 2
                                                }
                                            }

                                            TextField {
                                                id: newClientType
                                                Layout.fillWidth: true
                                                placeholderText: "수요처 유형"
                                                color: Colors.textPrimary
                                                font.pixelSize: 16

                                                background: Rectangle {
                                                    color: Colors.bgTertiary
                                                    radius: 8
                                                    border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary
                                                    border.width: 2
                                                }
                                            }

                                            Button {
                                                Layout.fillWidth: true
                                                text: "추가"
                                                enabled: newClientName.text.length > 0

                                                background: Rectangle {
                                                    color: parent.enabled ? Colors.btnGreen : Colors.btnGray
                                                    radius: 8
                                                }

                                                contentItem: Text {
                                                    text: parent.text
                                                    color: Colors.textPrimary
                                                    font.pixelSize: 16
                                                    horizontalAlignment: Text.AlignHCenter
                                                }

                                                onClicked: {
                                                    var newClient = {
                                                        id: Date.now(),
                                                        name: newClientName.text,
                                                        type: newClientType.text || "일반",
                                                        logo: ""
                                                    }
                                                    var arr = clients.slice()
                                                    arr.push(newClient)
                                                    clients = arr
                                                    dataManager.saveClients(clients)
                                                    selectedClient = newClient.name
                                                    newClientName.text = ""
                                                    newClientType.text = ""
                                                    showClientInput = false
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // 로고 경로
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Text {
                                text: "회사 로고 경로"
                                color: Colors.textSecondary
                                font.pixelSize: 18
                                font.bold: true
                            }

                            TextField {
                                Layout.fillWidth: true
                                placeholderText: "로고 이미지 경로 (예: C:/logo.png)"
                                color: Colors.textPrimary
                                font.pixelSize: 14
                                enabled: !root.emergencyStop
                                text: logoPreview
                                onTextChanged: logoPreview = text

                                background: Rectangle {
                                    color: Colors.bgTertiary
                                    radius: 8
                                    border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary
                                    border.width: 2
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 120
                                color: Colors.bgTertiary
                                radius: 8
                                visible: logoPreview !== ""

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    source: logoPreview !== "" ? ("file:///" + logoPreview) : ""
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                }
                            }
                        }

                        Item { Layout.fillHeight: true }

                        // 적용 버튼
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 70
                            text: "✓ 적용"
                            enabled: !root.emergencyStop

                            background: Rectangle {
                                color: parent.enabled ? Colors.btnGreen : Colors.btnGray
                                radius: 12
                            }

                            contentItem: Text {
                                text: parent.text
                                color: Colors.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            onClicked: {
                                root.onClientChange(selectedClient, logoPreview)
                                saveWorkplaceSettings()
                            }
                        }
                    }
                }

                // === 우측: 작업장 관리 ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 700
                    color: Colors.bgCard
                    radius: 16

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 32
                        spacing: 24

                        Text {
                            text: "작업장 정보"
                            color: Colors.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }

                        // 작업장소
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Text { text: "작업장소"; color: Colors.textSecondary; font.pixelSize: 18; font.bold: true }

                            ComboBox {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                enabled: !root.emergencyStop
                                model: locations
                                currentIndex: {
                                    for (var i=0;i<locations.length;i++)
                                        if(locations[i]===selectedLocation) return i
                                    return 0
                                }
                                onActivated: selectedLocation = locations[currentIndex]
                                delegate: ItemDelegate {
                                    width: parent.width
                                    contentItem: Text { text: modelData; color: Colors.textPrimary; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter }
                                    background: Rectangle { color: parent.highlighted ? Colors.btnBlue : Colors.bgTertiary }
                                }
                            }

                            // 새 작업장소 추가
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                TextField {
                                    id: newLocation
                                    Layout.fillWidth: true
                                    placeholderText: "새 작업장소 추가..."
                                    color: Colors.textPrimary
                                    font.pixelSize: 14
                                    background: Rectangle {
                                        color: Colors.bgTertiary; radius: 8
                                        border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary; border.width: 2
                                    }
                                }
                                Button {
                                    text: "+"
                                    enabled: newLocation.text.length > 0 && !root.emergencyStop
                                    background: Rectangle { color: parent.enabled ? Colors.btnGreen : Colors.btnGray; radius: 8 }
                                    contentItem: Text { text: parent.text; color: Colors.textPrimary; font.pixelSize: 18; horizontalAlignment: Text.AlignHCenter }
                                    onClicked: {
                                        var arr = locations.slice(); arr.push(newLocation.text)
                                        locations = arr; dataManager.saveLocations(locations)
                                        selectedLocation = newLocation.text; newLocation.text = ""
                                    }
                                }
                            }
                        }

                        // 작업업무
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Text { text: "작업업무"; color: Colors.textSecondary; font.pixelSize: 18; font.bold: true }

                            ComboBox {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                enabled: !root.emergencyStop
                                model: tasks
                                currentIndex: {
                                    for (var i=0;i<tasks.length;i++)
                                        if(tasks[i]===selectedTask) return i
                                    return 0
                                }
                                onActivated: selectedTask = tasks[currentIndex]
                                delegate: ItemDelegate {
                                    width: parent.width
                                    contentItem: Text { text: modelData; color: Colors.textPrimary; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter }
                                    background: Rectangle { color: parent.highlighted ? Colors.btnBlue : Colors.bgTertiary }
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                TextField {
                                    id: newTask
                                    Layout.fillWidth: true
                                    placeholderText: "새 작업업무 추가..."
                                    color: Colors.textPrimary; font.pixelSize: 14
                                    background: Rectangle {
                                        color: Colors.bgTertiary; radius: 8
                                        border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary; border.width: 2
                                    }
                                }
                                Button {
                                    text: "+"
                                    enabled: newTask.text.length > 0 && !root.emergencyStop
                                    background: Rectangle { color: parent.enabled ? Colors.btnGreen : Colors.btnGray; radius: 8 }
                                    contentItem: Text { text: parent.text; color: Colors.textPrimary; font.pixelSize: 18; horizontalAlignment: Text.AlignHCenter }
                                    onClicked: {
                                        var arr = tasks.slice(); arr.push(newTask.text)
                                        tasks = arr; dataManager.saveTasks(tasks)
                                        selectedTask = newTask.text; newTask.text = ""
                                    }
                                }
                            }
                        }

                        // 작업자
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Text { text: "작업자"; color: Colors.textSecondary; font.pixelSize: 18; font.bold: true }

                            ComboBox {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                enabled: !root.emergencyStop
                                model: workers
                                currentIndex: {
                                    for (var i=0;i<workers.length;i++)
                                        if(workers[i]===selectedWorker) return i
                                    return 0
                                }
                                onActivated: selectedWorker = workers[currentIndex]
                                delegate: ItemDelegate {
                                    width: parent.width
                                    contentItem: Text { text: modelData; color: Colors.textPrimary; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter }
                                    background: Rectangle { color: parent.highlighted ? Colors.btnBlue : Colors.bgTertiary }
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                TextField {
                                    id: newWorker
                                    Layout.fillWidth: true
                                    placeholderText: "새 작업자 추가..."
                                    color: Colors.textPrimary; font.pixelSize: 14
                                    background: Rectangle {
                                        color: Colors.bgTertiary; radius: 8
                                        border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary; border.width: 2
                                    }
                                }
                                Button {
                                    text: "+"
                                    enabled: newWorker.text.length > 0 && !root.emergencyStop
                                    background: Rectangle { color: parent.enabled ? Colors.btnGreen : Colors.btnGray; radius: 8 }
                                    contentItem: Text { text: parent.text; color: Colors.textPrimary; font.pixelSize: 18; horizontalAlignment: Text.AlignHCenter }
                                    onClicked: {
                                        var arr = workers.slice(); arr.push(newWorker.text)
                                        workers = arr; dataManager.saveWorkers(workers)
                                        selectedWorker = newWorker.text; newWorker.text = ""
                                    }
                                }
                            }
                        }

                        // 현재 설정 표시
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100
                            color: Qt.rgba(Colors.btnBlue.r, Colors.btnBlue.g, Colors.btnBlue.b, 0.2)
                            radius: 12
                            border.color: Colors.borderBlue
                            border.width: 2

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 8

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: "현재 프로젝트명"
                                    color: Colors.textTertiary
                                    font.pixelSize: 14
                                }

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: selectedLocation + "_" + selectedTask + "_" + selectedWorker
                                    color: Colors.textPrimary
                                    font.pixelSize: 20
                                    font.bold: true
                                }
                            }
                        }

                        Item { Layout.fillHeight: true }

                        // 적용 버튼
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 70
                            text: "✓ 적용"
                            enabled: !root.emergencyStop

                            background: Rectangle {
                                color: parent.enabled ? Colors.btnGreen : Colors.btnGray
                                radius: 12
                            }

                            contentItem: Text {
                                text: parent.text
                                color: Colors.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            onClicked: {
                                saveWorkplaceSettings()
                                console.log("작업장 정보 적용:", selectedLocation, selectedTask, selectedWorker)
                            }
                        }
                    }
                }
            }
        }
    }

}
