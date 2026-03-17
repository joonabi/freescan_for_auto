#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import openpyxl
from openpyxl.styles import Font, Alignment, Border, Side, PatternFill
from openpyxl.utils import get_column_letter

wb = openpyxl.Workbook()
ws = wb.active
ws.title = "입찰규격비교"

# ── Styles ──────────────────────────────────────────────────
thin   = Side(style='thin')
medium = Side(style='medium')

def bdr(l='thin', r='thin', t='thin', b='thin'):
    m = {'thin': thin, 'medium': medium, None: Side(style=None)}
    return Border(left=m[l], right=m[r], top=m[t], bottom=m[b])

def s(row, col, value='', bold=False, size=10, halign='center', valign='center',
      bg=None, wrap=True, border=True, indent=0):
    c = ws.cell(row=row, column=col, value=value)
    c.font = Font(name='맑은 고딕', bold=bold, size=size)
    c.alignment = Alignment(horizontal=halign, vertical=valign,
                            wrap_text=wrap, indent=indent)
    if bg:
        c.fill = PatternFill(fill_type='solid', fgColor=bg)
    if border:
        c.border = bdr()
    return c

def merge(ws, r1, c1, r2, c2):
    ws.merge_cells(start_row=r1, start_column=c1, end_row=r2, end_column=c2)

# ── Column widths ─────────────────────────────────────────────
# Col 1: 구분, 2: 세부항목, 3: 요구규격, 4~7: 업체별, 8: 비고
col_widths = {1: 12, 2: 16, 3: 30, 4: 24, 5: 24, 6: 24, 7: 24, 8: 8}
for col, w in col_widths.items():
    ws.column_dimensions[get_column_letter(col)].width = w

# ── Colors ────────────────────────────────────────────────────
TITLE_BG = '1F3864'   # 진남색
HEAD_BG  = '2E75B6'   # 파란색
CAT_BG   = 'DEEAF1'   # 매우 연파랑
REQ_BG   = 'FFF2CC'   # 연노랑 (요구규격)
PASS_BG  = 'E2EFDA'   # 연초록 (충족)
FAIL_BG  = 'FFDDC1'   # 연주황 (미충족)
UNKN_BG  = 'FFFFFF'   # 흰색 (미확인)

# ── 제조사별 데이터 ────────────────────────────────────────────
# 각 항목: (표시값, 충족여부)  True=충족(○ 녹색), False=미충족(✕ 주황), None=확인필요(흰색)

COMPANY_HEADERS = [
    "FARO\nOrbis Premium",
    "Leica\nBLK2GO PULSE",
    "NavVis\nVLX 3",
    "GeoSLAM\nZEB Horizon RT",
]

COMPANY_SPECS = [
    # ── FARO Orbis Premium ──────────────────────────────────
    [
        # 규격(본체)
        ("확인 필요", None),                          # 본체 크기
        ("약 3.6kg (전체 시스템)", False),             # 본체 무게  ✗
        # 규격(데이터로거)
        ("태블릿 형태 (별도 제공)", None),             # 데이터로거 크기
        ("배터리 포함", None),                         # 데이터로거 무게
        # 성능(일반측정)
        ("1등급", True),                               # 레이저 등급
        ("360° × 290°", True),                         # 시야각
        ("120m", True),                                # 측정거리
        ("640,000 pts/s", True),                       # 초당 포인트
        ("32채널", True),                              # LIDAR 채널
        ("5mm (SLAM 모드)", True),                     # 일반 정밀도
        ("보행 / 차량 / 정방향·측방향·역방향", True),  # 이동 측정
        # 성능(정밀측정)
        ("19M 포인트 (Flash)", True),                  # 해상도
        ("2mm (Flash 정밀 모드)", True),               # 정밀 정밀도
        ("72MP 360° (Insta360 선택)", True),           # 이미지 캡처
        ("실시간 완전 자동화 등록", True),             # 등록
        ("약 16초 (컬러 포함)", True),                 # 컬러 시간
        # 성능(이미지촬영)
        ("1대 (Insta360 선택 제공)", True),            # 360° 카메라
        # 성능(사용환경)
        ("실내 / 실외 이동 측정", True),               # 이동 측정 환경
        ("IP54", True),                                # IP 등급
        ("0°C ~ +40°C", True),                        # 사용 온도
        # 성능(운영방식)
        ("터치스크린", True),                          # 작동 방식
        ("탈착형 충전 배터리", True),                  # 전원 방식
        ("WiFi, RJ45, USB", True),                     # 통신
        ("512GB", True),                               # 저장
        ("약 4시간/충전 (교체 운용)", False),          # 사용시간  ✗
        # 기본구성(H/W)
        ("포함", True),                                # 장비 본체
        ("태블릿 데이터로거 포함", True),              # 데이터 로거
        ("포함", True),                                # 배터리
        ("포함", True),                                # 충전기
        ("선택사양", None),                            # 모노포드
        ("선택사양", None),                            # 외장 메모리
        ("선택사양", None),                            # 자동차 마운트
        # 기본구성(악세사리)
        ("선택사양", None),                            # 케이스
        ("선택사양", None),                            # 가방
    ],

    # ── Leica BLK2GO PULSE ─────────────────────────────────
    [
        # 규격(본체)
        ("282 × 80 × 80mm", None),                    # 본체 크기
        ("850g (배터리 포함)", True),                  # 본체 무게  ✓
        # 규격(데이터로거)
        ("앱 기반 (물리 로거 없음)", False),            # 데이터로거 크기  ✗
        ("해당없음", None),                             # 데이터로거 무게
        # 성능(일반측정)
        ("1등급", True),                               # 레이저 등급
        ("210° × 85°", False),                         # 시야각  ✗
        ("10m (실내 전용)", False),                    # 측정거리  ✗
        ("약 39,000 pts/s (ToF 방식)", False),         # 초당 포인트  ✗
        ("ToF 고체형 (2센서, 7,816 spot)", False),     # LIDAR 채널  ✗
        ("±20mm (SLAM)", False),                       # 일반 정밀도  ✗
        ("보행 가능 (차량 모드 없음)", False),          # 이동 측정  ✗
        # 성능(정밀측정)
        ("해당없음 (정밀 모드 없음)", False),           # 해상도  ✗
        ("±5mm (후처리 한정)", False),                 # 정밀 정밀도  ✗
        ("4.8MP × 3 (300° 파노라마)", False),          # 이미지 캡처  ✗
        ("GrandSLAM 자동 등록", True),                 # 등록
        ("연속 컬러화 (별도 캡처 없음)", None),        # 컬러 시간
        # 성능(이미지촬영)
        ("파노라마 카메라 3대 내장", True),             # 360° 카메라
        # 성능(사용환경)
        ("실내 위주 (단거리 제한)", True),             # 이동 측정 환경
        ("IP54", True),                                # IP 등급
        ("0°C ~ +40°C", True),                        # 사용 온도
        # 성능(운영방식)
        ("버튼 + 앱 연동 (BLK Live)", True),           # 작동 방식
        ("탈착형 GEB821 배터리", True),                # 전원 방식
        ("무선 (앱) + USB 3.0", False),               # 통신  ✗
        ("256GB", False),                              # 저장  ✗
        ("약 45분/충전", False),                       # 사용시간  ✗
        # 기본구성(H/W)
        ("포함", True),                                # 장비 본체
        ("앱 전용 (물리 로거 미제공)", False),          # 데이터 로거  ✗
        ("GEB821 × 3개 포함", True),                  # 배터리
        ("GKL825 멀티충전기 포함", True),              # 충전기
        ("미포함", False),                             # 모노포드  ✗
        ("미포함", False),                             # 외장 메모리  ✗
        ("미포함", False),                             # 자동차 마운트  ✗
        # 기본구성(악세사리)
        ("운반 케이스 포함", True),                    # 케이스
        ("손목 스트랩 (백팩 없음)", False),            # 가방  ✗
    ],

    # ── NavVis VLX 3 ───────────────────────────────────────
    [
        # 규격(본체)
        ("108.5 × 33 × 45cm (착용형)", False),        # 본체 크기  ✗
        ("8.5kg (2배터리 포함)", False),               # 본체 무게  ✗
        # 규격(데이터로거)
        ("일체형 (분리 불가)", False),                  # 데이터로거 크기  ✗
        ("해당없음", None),                             # 데이터로거 무게
        # 성능(일반측정)
        ("1등급", True),                               # 레이저 등급
        ("360° × ~360° (전방위)", True),               # 시야각
        ("300m (Hesai XT32M2X 센서)", True),           # 측정거리
        ("1,280,000 pts/s (듀얼 센서)", True),         # 초당 포인트
        ("2 × 32채널 = 64채널", True),                # LIDAR 채널
        ("5mm (SLAM 500m² 환경)", True),               # 일반 정밀도
        ("보행 가능 (차량 모드 없음)", False),          # 이동 측정  ✗
        # 성능(정밀측정)
        ("해당없음 (SLAM 전용)", False),               # 해상도  ✗
        ("~1cm 레인지 정밀도", False),                 # 정밀 정밀도  ✗
        ("4 × 20MP = 80MP 전방위", True),              # 이미지 캡처
        ("SLAM + GCP 자동 등록", True),                # 등록
        ("연속 컬러화 (별도 없음)", None),             # 컬러 시간
        # 성능(이미지촬영)
        ("4 × 20MP 카메라 내장", True),                # 360° 카메라
        # 성능(사용환경)
        ("실내 / 실외 이동 측정", True),               # 이동 측정 환경
        ("IP42", False),                               # IP 등급  ✗
        ("-10°C ~ +40°C", True),                       # 사용 온도
        # 성능(운영방식)
        ("5.5인치 터치스크린", True),                  # 작동 방식
        ("핫스왑 탈착형 배터리", True),                # 전원 방식
        ("WiFi + Bluetooth", False),                   # 통신  ✗
        ("1TB SSD", True),                             # 저장
        ("90분 (2배터리 세트 기준)", False),            # 사용시간  ✗
        # 기본구성(H/W)
        ("포함", True),                                # 장비 본체
        ("일체형 (분리 없음)", False),                  # 데이터 로거  ✗
        ("2개 배터리 포함", True),                     # 배터리
        ("충전기 포함", True),                         # 충전기
        ("미포함", False),                             # 모노포드  ✗
        ("미포함", False),                             # 외장 메모리  ✗
        ("미포함", False),                             # 자동차 마운트  ✗
        # 기본구성(악세사리)
        ("하드케이스 포함", True),                     # 케이스
        ("접이식 백팩 포함", True),                    # 가방
    ],

    # ── GeoSLAM ZEB Horizon RT ─────────────────────────────
    [
        # 규격(본체)
        ("공개 미확인", None),                         # 본체 크기
        ("1.45kg", True),                              # 본체 무게  ✓
        # 규격(데이터로거)
        ("공개 미확인", None),                         # 데이터로거 크기
        ("1.40kg (배터리 포함)", True),                # 데이터로거 무게  ✓
        # 성능(일반측정)
        ("1등급 (λ 903nm)", True),                    # 레이저 등급
        ("360° × 270°", False),                        # 시야각  ✗ (270 < 290)
        ("100m", False),                               # 측정거리  ✗
        ("300,000 pts/s", False),                      # 초당 포인트  ✗
        ("16채널 (Velodyne VLP-16)", False),           # LIDAR 채널  ✗
        ("6mm (상대정밀도)", False),                   # 일반 정밀도  ✗
        ("보행 / 차량 / 정방향·측방향·역방향", True),  # 이동 측정  ✓
        # 성능(정밀측정)
        ("해당없음 (SLAM 전용)", False),               # 해상도  ✗
        ("해당없음", False),                           # 정밀 정밀도  ✗
        ("ZEB Vision 선택사양", None),                 # 이미지 캡처
        ("GeoSLAM Beam (후처리)", False),              # 등록  ✗
        ("해당없음", False),                           # 컬러 시간  ✗
        # 성능(이미지촬영)
        ("ZEB Vision 선택사양", None),                 # 360° 카메라
        # 성능(사용환경)
        ("실내 / 실외 이동 측정", True),               # 이동 측정 환경
        ("IP54", True),                                # IP 등급
        ("확인 필요", None),                           # 사용 온도
        # 성능(운영방식)
        ("버튼 + 모바일 UI (Wi-Fi)", True),            # 작동 방식
        ("탈착형 14.8V 리튬이온", True),               # 전원 방식
        ("WiFi, Ethernet, USB 스틱", True),            # 통신
        ("110GB (데이터로거)", False),                 # 저장  ✗
        ("3.5시간/충전", False),                       # 사용시간  ✗
        # 기본구성(H/W)
        ("포함", True),                                # 장비 본체
        ("RT 데이터로거 포함", True),                  # 데이터 로거
        ("배터리 포함", True),                         # 배터리
        ("충전기 포함", True),                         # 충전기
        ("액세서리 봉(Pole) 포함", True),              # 모노포드 (봉으로 대체)
        ("USB 메모리 스틱 포함", True),                # 외장 메모리
        ("선택사양", None),                            # 자동차 마운트
        # 기본구성(악세사리)
        ("소프트 케이스 포함", True),                  # 케이스
        ("미포함", False),                             # 가방  ✗
    ],
]

# ── Row 1: 여백 ───────────────────────────────────────────────
ws.row_dimensions[1].height = 14

# ── Row 2: 제목 ───────────────────────────────────────────────
ws.row_dimensions[2].height = 40
merge(ws, 2, 1, 2, 8)
c = ws.cell(row=2, column=1,
            value='입찰 규격 비교표  |  핸드헬드 타입 현장재구성 측정 장비')
c.font = Font(name='맑은 고딕', bold=True, size=16, color='FFFFFF')
c.alignment = Alignment(horizontal='center', vertical='center')
c.fill = PatternFill(fill_type='solid', fgColor=TITLE_BG)
c.border = bdr()

# ── Row 3: 헤더 (구분 / 세부항목 / 요구규격 / 업체4개 / 비고) ──
ws.row_dimensions[3].height = 36
fixed_labels = ['구  분', '세부 항목', '요구 규격']
for col, label in enumerate(fixed_labels, 1):
    c = s(3, col, label, bold=True, size=10, bg=HEAD_BG)
    c.font = Font(name='맑은 고딕', bold=True, size=10, color='FFFFFF')

for i, header in enumerate(COMPANY_HEADERS):
    col = 4 + i
    c = s(3, col, header, bold=True, size=9, bg=HEAD_BG)
    c.font = Font(name='맑은 고딕', bold=True, size=9, color='FFFFFF')

c = s(3, 8, '비  고', bold=True, size=10, bg=HEAD_BG)
c.font = Font(name='맑은 고딕', bold=True, size=10, color='FFFFFF')

# ── 스펙 데이터 행 정의 ────────────────────────────────────────
specs = [
    # (구분, 세부항목, 요구규격)
    ('규격\n(본체)',        '크기',              '232mm × 410mm × 141mm 이하'),
    ('규격\n(본체)',        '무게',              '2.2kg 이하'),
    ('규격\n(데이터로거)',  '크기',              '197mm × 131mm × 91.6mm 이하\n(배터리 포함)'),
    ('규격\n(데이터로거)',  '무게',              '1.6kg 이하 (배터리 포함)'),
    ('성능\n(일반측정)',    '레이저 등급',        '1등급'),
    ('성능\n(일반측정)',    '시야각',            '360° × 290° 이상'),
    ('성능\n(일반측정)',    '측정거리',          '반경 120m 이상'),
    ('성능\n(일반측정)',    '초당 획득 포인트',   '최대 640,000 pts/s 이상'),
    ('성능\n(일반측정)',    'LIDAR 채널 수',     '32개 이상'),
    ('성능\n(일반측정)',    '정밀도',            '5mm 이상'),
    ('성능\n(일반측정)',    '이동 측정 방법',     '걸으며 이동 측정 가능\n자동차 고정 이동 측정 가능\n정방향·측방향·역방향 모두 측정'),
    ('성능\n(정밀측정)',    '해상도',            '19M 포인트 이상'),
    ('성능\n(정밀측정)',    '정밀도',            '2mm 이상'),
    ('성능\n(정밀측정)',    '이미지 캡처',        '측정 중 72MP의 360° 이미지 캡처'),
    ('성능\n(정밀측정)',    '등록',              '실시간 및 완전 자동화 등록'),
    ('성능\n(정밀측정)',    '컬러값 포함 기간',   '16초 이내'),
    ('성능\n(이미지촬영)',  '360° 카메라',       '1대 이상'),
    ('성능\n(사용환경)',    '이동 측정',         '실내 및 실외 이동하며 측정 가능'),
    ('성능\n(사용환경)',    '방진방수 등급',      'IP54 이상'),
    ('성능\n(사용환경)',    '사용 온도',         '0°C ~ +40°C'),
    ('성능\n(운영방식)',    '작동 방식',         '버튼 또는 터치스크린 작동'),
    ('성능\n(운영방식)',    '전원 방식',         '충전용 배터리 방식(탈착형)'),
    ('성능\n(운영방식)',    '통신 방식',         'WIFI, RJ54, USB 스틱으로 연결'),
    ('성능\n(운영방식)',    '저장장치',          '512GB 이상'),
    ('성능\n(운영방식)',    '사용 시간',         '최대 50시간 연속사용'),
    ('기본구성\n(H/W)',    '장비 본체',         '현장 동시 위치 측정 및 매핑 장비 본체 1대'),
    ('기본구성\n(H/W)',    '데이터 로거',        '1대'),
    ('기본구성\n(H/W)',    '배터리',            '1개'),
    ('기본구성\n(H/W)',    '충전기',            '1개'),
    ('기본구성\n(H/W)',    '모노포드',          '1개'),
    ('기본구성\n(H/W)',    '외장 메모리',        '1개'),
    ('기본구성\n(H/W)',    '자동차 마운트 세트', '1개'),
    ('기본구성\n(악세사리)', '휴대용 운영 케이스', '1개'),
    ('기본구성\n(악세사리)', '이동용 가방(백팩 등)', '1개'),
]

START_ROW = 4
prev_cat  = None
group_start = START_ROW

for i, (cat, item, req) in enumerate(specs):
    r = START_ROW + i
    h = 44 if '\n' in req and len(req) > 20 else (28 if '\n' in req else 22)
    ws.row_dimensions[r].height = h

    # 구분
    s(r, 1, cat if cat != prev_cat else '', bold=False, size=9,
      bg=CAT_BG, halign='center', valign='center')
    # 세부항목
    s(r, 2, item, size=9, halign='left', valign='center', indent=1)
    # 요구규격
    s(r, 3, req, size=9, halign='left', valign='center', bg=REQ_BG, indent=1)

    # 제조사별 스펙 채우기
    for j, cspecs in enumerate(COMPANY_SPECS):
        col = 4 + j
        val, ok = cspecs[i]
        if ok is True:
            bg = PASS_BG
            display = val + '  ○'
        elif ok is False:
            bg = FAIL_BG
            display = val + '  ✕'
        else:
            bg = UNKN_BG
            display = val
        s(r, col, display, size=9, halign='left', valign='center', bg=bg, indent=1)

    # 비고
    s(r, 8, '', size=9, halign='center', valign='center')

    prev_cat = cat

# ── 구분 셀 병합 ──────────────────────────────────────────────
prev_cat    = None
group_start = START_ROW
for i, (cat, _, _) in enumerate(specs):
    r = START_ROW + i
    if cat != prev_cat:
        if prev_cat is not None and r - 1 > group_start:
            merge(ws, group_start, 1, r - 1, 1)
            c = ws.cell(row=group_start, column=1)
            c.value = prev_cat
            c.font = Font(name='맑은 고딕', size=9, bold=True)
            c.alignment = Alignment(horizontal='center', vertical='center',
                                    wrap_text=True)
            c.fill = PatternFill(fill_type='solid', fgColor=CAT_BG)
            c.border = bdr()
        group_start = r
        prev_cat = cat

last_r = START_ROW + len(specs) - 1
if last_r > group_start:
    merge(ws, group_start, 1, last_r, 1)
    c = ws.cell(row=group_start, column=1)
    c.value = prev_cat
    c.font = Font(name='맑은 고딕', size=9, bold=True)
    c.alignment = Alignment(horizontal='center', vertical='center', wrap_text=True)
    c.fill = PatternFill(fill_type='solid', fgColor=CAT_BG)
    c.border = bdr()

# ── 범례 / 충족 개수 요약 ─────────────────────────────────────
legend_r = START_ROW + len(specs) + 1
ws.row_dimensions[legend_r - 1].height = 6   # 구분 여백

# 충족/미충족/미확인 개수 계산
for j, (hdr, cspecs) in enumerate(zip(COMPANY_HEADERS, COMPANY_SPECS)):
    pass_cnt = sum(1 for _, ok in cspecs if ok is True)
    fail_cnt = sum(1 for _, ok in cspecs if ok is False)
    unkn_cnt = sum(1 for _, ok in cspecs if ok is None)
    col = 4 + j
    summary_r = legend_r
    ws.row_dimensions[summary_r].height = 20
    merge(ws, summary_r, col, summary_r, col)
    summary_text = f'충족 {pass_cnt}  /  미충족 {fail_cnt}  /  미확인 {unkn_cnt}'
    c = ws.cell(row=summary_r, column=col, value=summary_text)
    c.font = Font(name='맑은 고딕', bold=True, size=8,
                  color='375623' if fail_cnt == 0 else '843C0C')
    c.alignment = Alignment(horizontal='center', vertical='center')
    c.fill = PatternFill(fill_type='solid', fgColor='F2F2F2')
    c.border = bdr()

# 범례 레이블
ws.row_dimensions[legend_r].height = 20
merge(ws, legend_r, 1, legend_r, 3)
c = ws.cell(row=legend_r, column=1, value='※ 항목별 충족 현황')
c.font = Font(name='맑은 고딕', bold=True, size=9)
c.alignment = Alignment(horizontal='left', vertical='center', indent=1)
c.fill = PatternFill(fill_type='solid', fgColor='F2F2F2')
c.border = bdr()

# 비고 칸
c = ws.cell(row=legend_r, column=8)
c.border = bdr()
c.fill = PatternFill(fill_type='solid', fgColor='F2F2F2')

# 작성 방법 안내
hints = [
    ('○ : 요구규격 충족 (연초록)',   PASS_BG),
    ('✕ : 요구규격 미충족 (연주황)',  FAIL_BG),
    ('미확인 : 공개 스펙 미발표 (흰색)', UNKN_BG),
]
for k, (hint_text, hint_bg) in enumerate(hints):
    hr = legend_r + 1 + k
    ws.row_dimensions[hr].height = 16
    merge(ws, hr, 1, hr, 8)
    c = ws.cell(row=hr, column=1, value=hint_text)
    c.font = Font(name='맑은 고딕', size=8, color='595959')
    c.alignment = Alignment(horizontal='left', vertical='center', indent=2)
    c.fill = PatternFill(fill_type='solid', fgColor=hint_bg)
    c.border = bdr()

# ── Page setup ────────────────────────────────────────────────
ws.page_setup.orientation = 'landscape'
ws.page_setup.paperSize   = 9   # A4
ws.page_setup.fitToPage   = True
ws.page_setup.fitToWidth  = 1
ws.page_setup.fitToHeight = 0
ws.page_margins.left   = 0.5
ws.page_margins.right  = 0.5
ws.page_margins.top    = 0.6
ws.page_margins.bottom = 0.6

# Freeze panes: 4행·4열 고정 (구분·세부항목·요구규격 항상 표시)
ws.freeze_panes = 'D4'

# ── Save ──────────────────────────────────────────────────────
import os
out_dir  = '/home/user/freescan_for_auto/3d_scan/police_korea'
os.makedirs(out_dir, exist_ok=True)
out_path = os.path.join(out_dir, '입찰규격비교.xlsx')
wb.save(out_path)
print(f'Saved: {out_path}')
