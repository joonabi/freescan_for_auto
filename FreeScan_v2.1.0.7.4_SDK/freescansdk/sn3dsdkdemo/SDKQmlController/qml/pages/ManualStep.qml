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
    property string currentSection: "intro"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // 좌측 목차
        Rectangle {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            color: Colors.bgCard
            border.color: Colors.borderPrimary
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    color: Colors.bgTertiary

                    Text {
                        anchors.centerIn: parent
                        text: "목차"
                        color: Colors.textPrimary
                        font.pixelSize: 24
                        font.bold: true
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ColumnLayout {
                        width: parent.width
                        spacing: 0

                        Repeater {
                            model: [
                                { id: "intro",       title: "시작하기",         icon: "🚀" },
                                { id: "basic",       title: "기본 사용법",       icon: "📱" },
                                { id: "scanner",     title: "3D 스캐너",        icon: "💻" },
                                { id: "connection",  title: "WiFi 연결",         icon: "📡" },
                                { id: "settings",    title: "설정 가이드",       icon: "⚙" },
                                { id: "troubleshoot",title: "문제 해결",         icon: "🔧" },
                                { id: "faq",         title: "자주 묻는 질문",    icon: "❓" }
                            ]

                            delegate: Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60

                                background: Rectangle {
                                    color: {
                                        if (root.currentSection === model.modelData.id) return Colors.btnBlue
                                        if (parent.hovered) return Colors.bgTertiary
                                        return "transparent"
                                    }
                                    Behavior on color { ColorAnimation { duration: 200 } }
                                }

                                contentItem: RowLayout {
                                    spacing: 12
                                    Text { text: model.modelData.icon; font.pixelSize: 24 }
                                    Text {
                                        Layout.fillWidth: true
                                        text: model.modelData.title
                                        color: Colors.textPrimary
                                        font.pixelSize: 16
                                        font.bold: root.currentSection === model.modelData.id
                                    }
                                }

                                onClicked: root.currentSection = model.modelData.id
                            }
                        }
                    }
                }
            }
        }

        // 우측 컨텐츠
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.parent.width
                spacing: 32
                padding: 40

                Loader {
                    Layout.fillWidth: true
                    sourceComponent: {
                        switch(root.currentSection) {
                            case "intro":        return introContent
                            case "basic":        return basicContent
                            case "scanner":      return scannerContent
                            case "connection":   return connectionContent
                            case "settings":     return settingsContent
                            case "troubleshoot": return troubleshootContent
                            case "faq":          return faqContent
                            default:             return introContent
                        }
                    }
                }
            }
        }
    }

    // 시작하기
    Component {
        id: introContent
        ColumnLayout {
            spacing: 24

            Text { text: "🚀 시작하기"; color: Colors.textPrimary; font.pixelSize: 36; font.bold: true }

            Text {
                Layout.fillWidth: true
                text: "3D FOR AUTOMATION에 오신 것을 환영합니다."
                color: Colors.textSecondary; font.pixelSize: 20; wrapMode: Text.WordWrap
            }

            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 2; color: Colors.borderPrimary }

            Text {
                Layout.fillWidth: true
                text: "본 시스템은 FreeScan UE Pro2 3D 스캐너를 WiFi로 연결하여 산업현장 자동화 스캔을 지원하는 컨트롤러입니다."
                color: Colors.textPrimary; font.pixelSize: 18; wrapMode: Text.WordWrap
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 16

                Text { text: "주요 기능"; color: Colors.textPrimary; font.pixelSize: 24; font.bold: true }

                Repeater {
                    model: [
                        "✓ FreeScan UE Pro2 WiFi 자동 연결",
                        "✓ 수요처 / 작업장 관리",
                        "✓ 실시간 스캔 상태 모니터링",
                        "✓ 자동 메시 변환 및 파일 저장",
                        "✓ 상세 SDK 로그 기록",
                        "✓ 설정 영구 저장 (INI 파일)"
                    ]
                    delegate: Text { text: modelData; color: Colors.textPrimary; font.pixelSize: 18 }
                }
            }
        }
    }

    // 기본 사용법
    Component {
        id: basicContent
        ColumnLayout {
            spacing: 24

            Text { text: "📱 기본 사용법"; color: Colors.textPrimary; font.pixelSize: 36; font.bold: true }

            ManualSection { title: "1. 작업장 관리"; content: "수요처 정보와 작업장 정보를 설정합니다. 작업자, 작업장소, 작업업무를 선택하고 '적용' 버튼을 클릭합니다.\n모든 설정은 자동으로 저장됩니다." }
            ManualSection { title: "2. 스캐너 연결"; content: "자동화 3D스캔 준비 페이지에서 WiFi IP와 포트를 입력한 후 '3D 스캐너 준비' 버튼을 클릭합니다.\n연결이 성공하면 상태가 녹색으로 변경됩니다." }
            ManualSection { title: "3. 설정 구성";   content: "자동화 설정 메뉴에서 스캔 파라미터와 후처리 옵션을 구성합니다.\n수동/자동 토글로 세부 설정을 제어할 수 있습니다." }
            ManualSection { title: "4. 스캔 시작";   content: "모든 설정이 완료되면 'AUTO 3D스캔 시작' 메뉴로 이동하여 스캔을 시작합니다.\n스캔 버튼 클릭 후 FreeScan 화면에서 스캔을 진행하면 자동으로 처리됩니다." }
        }
    }

    // 3D 스캐너
    Component {
        id: scannerContent
        ColumnLayout {
            spacing: 24

            Text { text: "💻 3D 스캐너"; color: Colors.textPrimary; font.pixelSize: 36; font.bold: true }

            ManualSection { title: "지원 모델";   content: "FreeScan UE Pro2\nSDK 버전: v2.1.0.7.4\n연결 방식: WiFi (Sn3DInitializeSocket / Sn3DConnectSocket)" }
            ManualSection { title: "스캔 플로우"; content: "Sn3DNewProject → Sn3DEnterScanPage → 스캔(FreeScan UI) → Sn3DEndScan → Sn3DMesh → Sn3DSaveMesh" }
            ManualSection { title: "스캔 모드";   content: "• 메쉬 스캔: 표면 정밀도 우선\n• 마커 스캔: 마커 기반 정렬, 대형 물체\n• 포토메트리: 텍스처 캡처" }
        }
    }

    // WiFi 연결
    Component {
        id: connectionContent
        ColumnLayout {
            spacing: 24

            Text { text: "📡 WiFi 연결"; color: Colors.textPrimary; font.pixelSize: 36; font.bold: true }

            ManualSection {
                title: "연결 방법"
                content: "1. FreeScan UE Pro2 전원을 켭니다\n2. 스캐너의 WiFi AP에 PC를 연결합니다\n3. '자동화 3D스캔 준비' 페이지로 이동합니다\n4. IP 주소와 포트를 입력합니다\n   (기본값: 192.168.1.100 / 11080)\n5. '3D 스캐너 준비' 버튼을 클릭합니다"
            }

            ManualSection {
                title: "기본 네트워크 설정"
                content: "스캐너 기본 IP: 192.168.1.100\n포트: 11080\n\n네트워크 구성이 다를 경우 스캐너 설정에서 확인하세요."
            }

            ManualSection {
                title: "주의사항"
                content: "• PC와 스캐너가 같은 네트워크에 있어야 합니다\n• 방화벽에서 포트 11080이 허용되어야 합니다\n• 연결 타임아웃: 30초"
            }
        }
    }

    // 설정 가이드
    Component {
        id: settingsContent
        ColumnLayout {
            spacing: 24

            Text { text: "⚙ 설정 가이드"; color: Colors.textPrimary; font.pixelSize: 36; font.bold: true }

            ManualSection {
                title: "수동/자동 토글"
                content: "각 설정 항목은 수동 또는 자동 모드로 전환할 수 있습니다.\n• 수동: 기본값 사용, 세부 설정 숨김\n• 자동: 세부 설정 표시 및 조정 가능"
            }

            ManualSection {
                title: "권장 설정"
                content: "일반 공업 부품:\n• 스캔 모드: 메쉬 스캔\n• 레이저 라인수: 크로스\n• 대상물 재질: 일반\n• 필터: 2단계\n• 스무스: 1단계\n• 파일 형식: STL"
            }

            ManualSection {
                title: "저장 위치"
                content: "설정은 실행 폴더의 qmlcontroller.ini 파일에 저장됩니다.\n스캔 결과 파일은 '저장 경로'에 설정된 폴더에 저장됩니다."
            }
        }
    }

    // 문제 해결
    Component {
        id: troubleshootContent
        ColumnLayout {
            spacing: 24

            Text { text: "🔧 문제 해결"; color: Colors.textPrimary; font.pixelSize: 36; font.bold: true }

            ManualSection { title: "Q: 스캐너 연결이 안 돼요";   content: "A: 다음 사항을 확인하세요:\n1. 스캐너 전원 확인\n2. WiFi 네트워크 연결 상태\n3. IP/포트 설정 확인\n4. 방화벽 설정 (포트 11080)\n5. '다시 시도' 버튼 클릭" }
            ManualSection { title: "Q: 스캔이 중간에 멈춰요";    content: "A: 비상 정지 버튼이 눌렸는지 확인하고, '재시작' 버튼을 클릭하세요.\nFreeScan 화면에서 스캔 상태를 확인하세요." }
            ManualSection { title: "Q: 파일 저장이 실패해요";    content: "A: 디스크 공간과 저장 경로 권한을 확인하세요.\n설정 페이지에서 저장 경로를 변경할 수 있습니다." }
            ManualSection { title: "Q: 메시 변환이 느려요";      content: "A: 스캔 데이터 양에 따라 메시 변환 시간이 다를 수 있습니다.\n필터 단계를 낮추면 처리 속도가 향상됩니다." }
        }
    }

    // FAQ
    Component {
        id: faqContent
        ColumnLayout {
            spacing: 24

            Text { text: "❓ 자주 묻는 질문"; color: Colors.textPrimary; font.pixelSize: 36; font.bold: true }

            ManualSection { title: "비상 정지 버튼은 어떻게 사용하나요?"; content: "좌측 사이드바 하단의 빨간색 '긴급 정지' 버튼을 클릭하면 스캔이 즉시 중지됩니다." }
            ManualSection { title: "수요처 로고를 변경하려면?";           content: "'작업장 관리' 메뉴에서 '파일 선택' 버튼을 클릭하여 이미지를 업로드하세요." }
            ManualSection { title: "로그는 어디서 확인하나요?";           content: "좌측 메뉴의 'Log 정보'를 클릭하면 SDK 통신 로그를 실시간으로 확인할 수 있습니다." }
            ManualSection { title: "시뮬레이션 모드란?";                  content: "스캐너가 연결되지 않은 경우 자동으로 시뮬레이션 모드로 실행됩니다.\n실제 스캔 흐름을 테스트할 수 있습니다." }
        }
    }
}

// 매뉴얼 섹션 컴포넌트
component ManualSection: Rectangle {
    property string title: ""
    property string content: ""

    Layout.fillWidth: true
    Layout.preferredHeight: contentColumn.height + 40
    color: Colors.bgCard
    radius: 12

    ColumnLayout {
        id: contentColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20
        spacing: 12

        Text {
            text: title
            color: Colors.textPrimary
            font.pixelSize: 20
            font.bold: true
        }

        Text {
            Layout.fillWidth: true
            text: content
            color: Colors.textSecondary
            font.pixelSize: 16
            wrapMode: Text.WordWrap
            lineHeight: 1.5
        }
    }
}
