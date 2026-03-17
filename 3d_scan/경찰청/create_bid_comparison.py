#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import openpyxl
from openpyxl.styles import Font, Alignment, Border, Side, PatternFill
from openpyxl.utils import get_column_letter

wb = openpyxl.Workbook()
ws = wb.active
ws.title = "입찰규격비교"

# ── Styles ──────────────────────────────────────────────────
thin  = Side(style='thin')
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

# ── Column widths ────────────────────────────────────────────
col_widths = {1: 12, 2: 14, 3: 32, 4: 22, 5: 22, 6: 22, 7: 22, 8: 10}
for col, w in col_widths.items():
    ws.column_dimensions[get_column_letter(col)].width = w

# ── Colors ───────────────────────────────────────────────────
TITLE_BG   = '1F3864'   # 진남색
HEAD_BG    = '2E75B6'   # 파란색
SUB_BG     = 'BDD7EE'   # 연파랑
CAT_BG     = 'DEEAF1'   # 매우 연파랑
REQ_BG     = 'FFF2CC'   # 연노랑 (요구규격)
INPUT_BG   = 'FFFFFF'   # 흰색 (입력란)
PASS_BG    = 'E2EFDA'   # 연초록
FAIL_BG    = 'FFDDC1'   # 연주황

# ── Row 1: 제목 ──────────────────────────────────────────────
ws.row_dimensions[1].height = 14

ws.row_dimensions[2].height = 40
merge(ws, 2, 1, 2, 8)
c = ws.cell(row=2, column=1,
            value='입찰 규격 비교표  |  핸드헬드 타입 현장재구성 측정 장비')
c.font = Font(name='맑은 고딕', bold=True, size=16, color='FFFFFF')
c.alignment = Alignment(horizontal='center', vertical='center')
c.fill = PatternFill(fill_type='solid', fgColor=TITLE_BG)
c.border = bdr()

# ── Row 3: 업체명 입력란 ─────────────────────────────────────
ws.row_dimensions[3].height = 22
labels = ['구  분', '세부 항목', '요구 규격',
          '업체A\n제품명 입력', '업체B\n제품명 입력',
          '업체C\n제품명 입력', '업체D\n제품명 입력', '비  고']
for col, label in enumerate(labels, 1):
    c = s(3, col, label, bold=True, size=10, bg=HEAD_BG)
    c.font = Font(name='맑은 고딕', bold=True, size=10, color='FFFFFF')

# ── Data rows ────────────────────────────────────────────────

specs = [
    # (구분, 세부항목, 요구규격)
    # ── 2.2 규격 ──
    ('규격\n(본체)',      '크기',             '232mm × 410mm × 141mm 이하'),
    ('규격\n(본체)',      '무게',             '2.2kg 이하'),
    ('규격\n(데이터로거)', '크기',             '197mm × 131mm × 91.6mm 이하\n(배터리 포함)'),
    ('규격\n(데이터로거)', '무게',             '1.6kg 이하 (배터리 포함)'),
    # ── 2.3 성능 – 일반측정 ──
    ('성능\n(일반측정)',  '레이저 등급',       '1등급'),
    ('성능\n(일반측정)',  '시야각',           '360° × 290° 이상'),
    ('성능\n(일반측정)',  '측정거리',         '반경 120m 이상'),
    ('성능\n(일반측정)',  '초당 획득 포인트',  '최대 640,000 pts/s 이상'),
    ('성능\n(일반측정)',  'LIDAR 채널 수',    '32개 이상'),
    ('성능\n(일반측정)',  '정밀도',           '5mm 이상'),
    ('성능\n(일반측정)',  '이동 측정 방법',    '걸으며 이동 측정 가능\n자동차 고정 이동 측정 가능\n정방향·측방향·역방향 모두 측정'),
    # ── 2.3 성능 – 정밀측정 ──
    ('성능\n(정밀측정)',  '해상도',           '19M 포인트 이상'),
    ('성능\n(정밀측정)',  '정밀도',           '2mm 이상'),
    ('성능\n(정밀측정)',  '이미지 캡처',       '측정 중 72MP의 360° 이미지 캡처'),
    ('성능\n(정밀측정)',  '등록',             '실시간 및 완전 자동화 등록'),
    ('성능\n(정밀측정)',  '컬러값 포함 기간',  '16초 이내'),
    # ── 이미지 촬영 ──
    ('성능\n(이미지촬영)', '360° 카메라',     '1대 이상'),
    # ── 사용 환경 ──
    ('성능\n(사용환경)',  '이동 측정',        '실내 및 실외 이동하며 측정 가능'),
    ('성능\n(사용환경)',  '방진방수 등급',     'IP54 이상'),
    ('성능\n(사용환경)',  '사용 온도',        '0°C ~ +40°C'),
    # ── 운영 방식 ──
    ('성능\n(운영방식)',  '작동 방식',        '버튼 또는 터치스크린 작동'),
    ('성능\n(운영방식)',  '전원 방식',        '충전용 배터리 방식(탈착형)'),
    ('성능\n(운영방식)',  '통신 방식',        'WIFI, RJ54, USB 스틱으로 연결'),
    ('성능\n(운영방식)',  '저장장치',         '512GB 이상'),
    ('성능\n(운영방식)',  '사용 시간',        '최대 50시간 연속사용'),
    # ── 기본 구성 H/W ──
    ('기본구성\n(H/W)',  '장비 본체',        '현장 동시 위치 측정 및 매핑 장비 본체 1대'),
    ('기본구성\n(H/W)',  '데이터 로거',      '1대'),
    ('기본구성\n(H/W)',  '배터리',           '1개'),
    ('기본구성\n(H/W)',  '충전기',           '1개'),
    ('기본구성\n(H/W)',  '모노포드',         '1개'),
    ('기본구성\n(H/W)',  '외장 메모리',      '1개'),
    ('기본구성\n(H/W)',  '자동차 마운트 세트', '1개'),
    # ── 악세사리 ──
    ('기본구성\n(악세사리)', '휴대용 운영 케이스', '1개'),
    ('기본구성\n(악세사리)', '이동용 가방(백팩 등)', '1개'),
]

START_ROW = 4
prev_cat = None
cat_start_row = START_ROW

row_heights = []

for i, (cat, item, req) in enumerate(specs):
    r = START_ROW + i
    # row height: 이동측정 항목은 좀 더 높게
    h = 42 if '\n' in req and len(req) > 20 else (28 if '\n' in req else 22)
    ws.row_dimensions[r].height = h

    # 구분 셀 (나중에 merge)
    s(r, 1, cat if cat != prev_cat else '', bold=False, size=9,
      bg=CAT_BG, halign='center', valign='center')

    s(r, 2, item, size=9, halign='left', valign='center', indent=1)
    s(r, 3, req, size=9, halign='left', valign='center', bg=REQ_BG, indent=1)

    # 업체 입력란 (4개)
    for col in range(4, 8):
        s(r, col, '', size=9, bg=INPUT_BG, halign='center', valign='center')

    # 비고
    s(r, 8, '', size=9, halign='center', valign='center')

    prev_cat = cat

# ── 구분 셀 병합 ────────────────────────────────────────────
categories_order = []
seen = {}
for i, (cat, _, _) in enumerate(specs):
    r = START_ROW + i
    if cat not in seen:
        seen[cat] = r
    seen[cat + '_end'] = r

# merge by contiguous groups
prev_cat = None
group_start = START_ROW
for i, (cat, _, _) in enumerate(specs):
    r = START_ROW + i
    if cat != prev_cat:
        if prev_cat is not None and r - 1 > group_start:
            merge(ws, group_start, 1, r - 1, 1)
            # re-apply style to merged cell
            c = ws.cell(row=group_start, column=1)
            c.value = prev_cat
            c.font = Font(name='맑은 고딕', size=9, bold=True)
            c.alignment = Alignment(horizontal='center', vertical='center',
                                    wrap_text=True)
            c.fill = PatternFill(fill_type='solid', fgColor=CAT_BG)
            c.border = bdr()
        group_start = r
        prev_cat = cat

# last group
last_r = START_ROW + len(specs) - 1
if last_r > group_start:
    merge(ws, group_start, 1, last_r, 1)
    c = ws.cell(row=group_start, column=1)
    c.value = prev_cat
    c.font = Font(name='맑은 고딕', size=9, bold=True)
    c.alignment = Alignment(horizontal='center', vertical='center', wrap_text=True)
    c.fill = PatternFill(fill_type='solid', fgColor=CAT_BG)
    c.border = bdr()

# ── 적합/부적합 범례 행 ────────────────────────────────────
legend_r = START_ROW + len(specs) + 1
ws.row_dimensions[legend_r].height = 18
ws.row_dimensions[legend_r + 1].height = 18

merge(ws, legend_r, 1, legend_r, 3)
c = ws.cell(row=legend_r, column=1, value='※ 작성 방법')
c.font = Font(name='맑은 고딕', bold=True, size=9)
c.alignment = Alignment(horizontal='left', vertical='center', indent=1)

hints = [
    '① 업체명/제품명을 3행 업체A~D 셀에 직접 입력하세요.',
    '② 각 항목의 업체 사양을 직접 입력하세요.',
    '③ 요구규격 충족 여부 : 충족 → ○  /  미충족 → ✕  를 비고란에 표시하세요.',
]
for j, hint in enumerate(hints):
    r = legend_r + j
    ws.row_dimensions[r].height = 16
    merge(ws, r, 1, r, 8)
    c = ws.cell(row=r, column=1, value=hint)
    c.font = Font(name='맑은 고딕', size=9, color='595959')
    c.alignment = Alignment(horizontal='left', vertical='center', indent=2)

# ── Page setup ───────────────────────────────────────────────
ws.page_setup.orientation = 'landscape'
ws.page_setup.paperSize = 9   # A4
ws.page_setup.fitToPage = True
ws.page_setup.fitToWidth = 1
ws.page_setup.fitToHeight = 0
ws.page_margins.left = 0.5
ws.page_margins.right = 0.5
ws.page_margins.top = 0.6
ws.page_margins.bottom = 0.6

# Freeze panes at row 4, col 4
ws.freeze_panes = 'D4'

# ── Save ─────────────────────────────────────────────────────
import os
out_dir = '/home/user/freescan_for_auto/3d_scan/경찰청'
os.makedirs(out_dir, exist_ok=True)
out_path = os.path.join(out_dir, '입찰규격비교.xlsx')
wb.save(out_path)
print(f'Saved: {out_path}')
