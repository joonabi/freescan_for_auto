pragma Singleton
import QtQuick 2.15

QtObject {
    // 배경 색상
    readonly property color bgPrimary: "#0a0a0a"      // gray-950
    readonly property color bgSecondary: "#1f2937"    // gray-800
    readonly property color bgTertiary: "#111827"     // gray-900
    readonly property color bgCard: "#1f2937"         // gray-800

    // 버튼 색상
    readonly property color btnBlue: "#2563eb"        // blue-600
    readonly property color btnBlueHover: "#1d4ed8"   // blue-700
    readonly property color btnGreen: "#16a34a"       // green-600
    readonly property color btnGreenHover: "#15803d"  // green-700
    readonly property color btnRed: "#dc2626"         // red-600
    readonly property color btnRedHover: "#b91c1c"    // red-700
    readonly property color btnYellow: "#ca8a04"      // yellow-600
    readonly property color btnYellowHover: "#a16207" // yellow-700
    readonly property color btnGray: "#4b5563"        // gray-600

    // 텍스트 색상
    readonly property color textPrimary: "#ffffff"
    readonly property color textSecondary: "#d1d5db"  // gray-300
    readonly property color textTertiary: "#9ca3af"   // gray-400
    readonly property color textDisabled: "#6b7280"   // gray-500

    // 테두리 색상
    readonly property color borderPrimary: "#374151"  // gray-700
    readonly property color borderSecondary: "#4b5563" // gray-600
    readonly property color borderBlue: "#2563eb"
    readonly property color borderGreen: "#16a34a"
    readonly property color borderRed: "#dc2626"
    readonly property color borderYellow: "#ca8a04"

    // 상태 색상
    readonly property color success: "#10b981"        // green-500
    readonly property color warning: "#f59e0b"        // yellow-500
    readonly property color error: "#ef4444"          // red-500
    readonly property color info: "#3b82f6"           // blue-500

    // 그라데이션
    readonly property color gradientBlueStart: "#3b82f6"
    readonly property color gradientBlueEnd: "#06b6d4"
    readonly property color gradientGreenStart: "#10b981"
    readonly property color gradientGreenEnd: "#059669"
    readonly property color gradientRedStart: "#ef4444"
    readonly property color gradientRedEnd: "#dc2626"
    readonly property color gradientYellowStart: "#f59e0b"
    readonly property color gradientYellowEnd: "#d97706"
}
