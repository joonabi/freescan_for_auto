#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import openpyxl
from openpyxl.styles import (
    Font, Alignment, Border, Side, PatternFill, numbers
)
from openpyxl.utils import get_column_letter

wb = openpyxl.Workbook()
ws = wb.active
ws.title = "물품 규격서"

# --- Styles ---
thin = Side(style='thin')
medium = Side(style='medium')

def border(left='thin', right='thin', top='thin', bottom='thin'):
    sides = {'thin': thin, 'medium': medium, None: Side(style=None)}
    return Border(
        left=sides.get(left, thin),
        right=sides.get(right, thin),
        top=sides.get(top, thin),
        bottom=sides.get(bottom, thin)
    )

def cell_style(ws, row, col, value, bold=False, size=11, halign='center', valign='center',
               bg=None, border_style=True, wrap=True, font_size=None):
    c = ws.cell(row=row, column=col, value=value)
    c.font = Font(name='맑은 고딕', bold=bold, size=font_size or size)
    c.alignment = Alignment(horizontal=halign, vertical=valign, wrap_text=wrap)
    if bg:
        c.fill = PatternFill(fill_type='solid', fgColor=bg)
    if border_style:
        c.border = border()
    return c

# Column widths
ws.column_dimensions['A'].width = 14
ws.column_dimensions['B'].width = 14
ws.column_dimensions['C'].width = 40
ws.column_dimensions['D'].width = 10
ws.column_dimensions['E'].width = 12

# ============================================================
# 제목
# ============================================================
ws.row_dimensions[1].height = 10
ws.row_dimensions[2].height = 36
ws.merge_cells('A2:E2')
c = ws['A2']
c.value = '물품 규격서'
c.font = Font(name='맑은 고딕', bold=True, size=18)
c.alignment = Alignment(horizontal='center', vertical='center')

# ============================================================
# 물품 기본 정보 테이블 (row 3~5)
# ============================================================
header_bg = 'D9D9D9'

ws.row_dimensions[3].height = 20
ws.row_dimensions[4].height = 20
ws.row_dimensions[5].height = 30

# Row 3 headers
for col, val in enumerate(['관세분류번호', '품목번호', '품명', '단위', '수량'], 1):
    cell_style(ws, 3, col, val, bold=True, bg=header_bg)

# Row 4 English headers
for col, val in enumerate(['HSK CODE', 'ITEM NO.', 'DESCRIPTION', 'UNIT', 'QUANTITY'], 1):
    cell_style(ws, 4, col, val, bold=True, bg=header_bg)

# Row 5 data
cell_style(ws, 5, 1, '')
cell_style(ws, 5, 2, '')
cell_style(ws, 5, 3, '핸드헬드 타입 현장재구성 측정 장비', halign='center')
cell_style(ws, 5, 4, '대', halign='center')
cell_style(ws, 5, 5, 3, halign='center')

# ============================================================
# 섹션 헤더 helper
# ============================================================
row = 7

def section_title(ws, row, text, level=1):
    ws.row_dimensions[row].height = 22
    ws.merge_cells(f'A{row}:E{row}')
    c = ws.cell(row=row, column=1, value=text)
    if level == 1:
        c.font = Font(name='맑은 고딕', bold=True, size=12)
    else:
        c.font = Font(name='맑은 고딕', bold=True, size=11)
    c.alignment = Alignment(horizontal='left', vertical='center', indent=1)
    return row + 1

def text_row(ws, row, text, indent=2):
    ws.row_dimensions[row].height = 18
    ws.merge_cells(f'A{row}:E{row}')
    c = ws.cell(row=row, column=1, value=text)
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='left', vertical='center',
                            wrap_text=True, indent=indent)
    return row + 1

# ============================================================
# 1. 적용 범위 및 분류
# ============================================================
row = section_title(ws, row, '1. 적용 범위 및 분류')
row = section_title(ws, row, '1.1. 적용 범위', level=2)
row = text_row(ws, row,
    '화재, 안전사고 등 사건현장에서 신속하고 훼손없이 3차원으로 재구성 하는 장비', indent=3)
ws.row_dimensions[row-1].height = 24
row = section_title(ws, row, '1.2. 분류 : 과학수사 장비 중 분석장비', level=2)

# ============================================================
# 2. 구조 및 치수
# ============================================================
row += 1
row = section_title(ws, row, '2. 구조 및 치수')
row = section_title(ws, row, '2.1. 형태', level=2)
row = text_row(ws, row,
    '신속성·편의성 향상을 위한 핸드헬드 타입으로 이동 시 레이저 포인트를 이용하여 재구성 가능 형태',
    indent=3)
ws.row_dimensions[row-1].height = 30

# 2.2 규격 테이블
row += 1
row = section_title(ws, row, '2.2. 규격', level=2)

ws.row_dimensions[row].height = 20
for col, (val, w) in enumerate(zip(['구  분', '', '규  격', '비  고'], [1,2,3,4]), 1):
    cell_style(ws, row, col, val if col != 2 else '', bold=True, bg=header_bg)
ws.merge_cells(f'A{row}:B{row}')
ws.merge_cells(f'C{row}:D{row}')
ws.cell(row=row, column=5).border = border()
ws.cell(row=row, column=5).fill = PatternFill(fill_type='solid', fgColor=header_bg)
ws.cell(row=row, column=5).font = Font(name='맑은 고딕', bold=True)
ws.cell(row=row, column=5).alignment = Alignment(horizontal='center', vertical='center')

spec_22 = [
    ('본체', '크기', '232mm × 410mm × 141mm 이하', ''),
    ('본체', '무게', '2.2kg 이하', ''),
    ('데이터로거', '크기', '197mm × 131mm × 91.6mm 이하', '배터리 포함'),
    ('데이터로거', '무게', '1.6kg 이하', '배터리 포함'),
]

prev_category = None
category_start = None

for i, (category, item, spec, note) in enumerate(spec_22):
    r = row + 1 + i
    ws.row_dimensions[r].height = 18
    cell_style(ws, r, 1, category if category != prev_category else '')
    cell_style(ws, r, 2, item, halign='center')
    cell_style(ws, r, 3, spec, halign='left')
    ws.merge_cells(f'C{r}:D{r}')
    cell_style(ws, r, 5, note, halign='center')
    prev_category = category

# Merge category cells
row += 1
ws.merge_cells(f'A{row}:A{row+1}')   # 본체
ws.merge_cells(f'A{row+2}:A{row+3}') # 데이터로거
row += 4

# ============================================================
# 2.3 성능 테이블
# ============================================================
row += 1
row = section_title(ws, row, '2.3. 성능', level=2)

ws.row_dimensions[row].height = 20
cell_style(ws, row, 1, '구  분', bold=True, bg=header_bg)
ws.merge_cells(f'B{row}:D{row}')
cell_style(ws, row, 2, '성  능', bold=True, bg=header_bg)
cell_style(ws, row, 5, '비  고', bold=True, bg=header_bg)
# fill remaining
for col in [3, 4]:
    c = ws.cell(row=row, column=col)
    c.fill = PatternFill(fill_type='solid', fgColor=header_bg)
    c.border = border()

perf_data = [
    ('일반 측정', [
        '- 레이저등급 : 1등급',
        '- 시야각 : 360° x 290° 이상',
        '- 측정거리 : 반경 120m 이상',
        '- 초당 획득 포인트 : 최대 640,000 pts/s 이상',
        '- LIDAR 채널 수 : 32개 이상',
        '- 정밀도 : 5mm 이상',
        '- 걸으며 이동하면서 측정',
        '- 자동차에 고정하여 이동하면서 측정',
        '- 정방향, 측방향, 역방향 모두 측정 가능',
    ]),
    ('정밀 측정', [
        '- 해상도 : 19M 포인트 이상',
        '- 정밀도 : 2mm 이상',
        '- 이미지 : 측정 중 72MP의 360° 이미지 캡처',
        '- 등록 : 실시간 및 완전 자동화 등록',
        '- 기간 : 16초 이내 컬러값 포함',
    ]),
    ('이미지 촬영', [
        '- 360° 카메라 1대 이상',
    ]),
    ('사용 환경', [
        '- 실내 및 실외 이동하며 측정 가능',
        '- 방진방수등급 : IP54 이상',
        '- 사용온도 : 0°C ~ +40°C',
    ]),
    ('운영 방식', [
        '- 작동방식 : 버튼 또는 터치스크린 작동',
        '- 전원방식 : 충전용 배터리 방식(탈착형)',
        '- 통신방식 : WIFI, RJ54, USB 스틱으로 연결',
        '- 저장장치 : 512GB 이상',
        '- 사용시간 : 최대 50시간 연속사용',
    ]),
]

row += 1
for category, items in perf_data:
    start_r = row
    for j, item in enumerate(items):
        r = row + j
        ws.row_dimensions[r].height = 18
        cell_style(ws, r, 1, category if j == 0 else '', halign='center')
        ws.merge_cells(f'B{r}:D{r}')
        cell_style(ws, r, 2, item, halign='left')
        # fill merged
        for col in [3, 4]:
            c = ws.cell(row=r, column=col)
            c.border = border()
        cell_style(ws, r, 5, '', halign='center')
    if len(items) > 1:
        ws.merge_cells(f'A{start_r}:A{start_r+len(items)-1}')
    row += len(items)

# ============================================================
# 3. 기본 구성
# ============================================================
row += 1
row = section_title(ws, row, '3. 기본 구성')
row = section_title(ws, row, '3.1. 하드웨어(H/W)', level=2)

hw_items = [
    ('3.1.1', '현장 동시 위치 측정 및 매핑 장비 본체', '1대'),
    ('3.1.2', '데이터 로거', '1대'),
    ('3.1.3', '배터리', '1개'),
    ('3.1.4', '충전기', '1개'),
    ('3.1.5', '모노포드', '1개'),
    ('3.1.6', '외장 메모리', '1개'),
    ('3.1.7', '자동차 마운트 세트', '1개'),
]

for num, desc, qty in hw_items:
    ws.row_dimensions[row].height = 18
    ws.merge_cells(f'A{row}:B{row}')
    ws.merge_cells(f'C{row}:D{row}')
    c = ws.cell(row=row, column=1, value=f'  {num}')
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='left', vertical='center')
    c = ws.cell(row=row, column=3, value=desc)
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='left', vertical='center')
    c = ws.cell(row=row, column=5, value=qty)
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='center', vertical='center')
    row += 1

row = section_title(ws, row, '3.2. 악세사리', level=2)
acc_items = [
    ('3.2.1', '휴대용 운영 케이스', '1개'),
    ('3.2.2', '이동용 가방(백팩 등)', '1개'),
]
for num, desc, qty in acc_items:
    ws.row_dimensions[row].height = 18
    ws.merge_cells(f'A{row}:B{row}')
    ws.merge_cells(f'C{row}:D{row}')
    c = ws.cell(row=row, column=1, value=f'  {num}')
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='left', vertical='center')
    c = ws.cell(row=row, column=3, value=desc)
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='left', vertical='center')
    c = ws.cell(row=row, column=5, value=qty)
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='center', vertical='center')
    row += 1

# ============================================================
# 4. 검사와 샘플
# ============================================================
row += 1
row = section_title(ws, row, '4. 검사와 샘플')
row = section_title(ws, row, '4.1. 검사', level=2)
row = section_title(ws, row, '4.1.1. 규격서에 의해 검사를 받아야 하며, 납품 후 1주일간 시범운영 결과 특이사항 없을 시, 검사확인', level=2)
ws.row_dimensions[row-1].height = 36

# ============================================================
# 5. 물품의 배송 및 유지보수
# ============================================================
row += 1
row = section_title(ws, row, '5. 물품의 배송 및 유지보수')

delivery_items = [
    ('5.1', '경찰청에서 지정한 곳에 각 배송되어야 함 (계약 후 물품배부처 제공)'),
    ('5.2', '운송 중이나 검수 이전에 발생된 제품하자에 대해서는 계약자가 즉시 신제품으로 교환, 배송하여야하며 해당 운송료는 납품업자가 부담하여야 한다.'),
    ('5.3', '제품에 대한 무상A/S기간은 1년으로 하며, 국내에서 신속히 처리 가능하여야 함\n※ 각 시도경찰청별 AS 가능장소·연락처를 첨부하여야 함'),
    ('5.4', '단위포장상자 겉면에는 품명, 제조년월일, 제조회사명, 전화번호 및 제품 작동요령 등을 표시하고 외부 포장상자 겉면에는 품명, 수량, 제조년도, 제조회사명을 선명하게 표시한다.'),
]

for num, text in delivery_items:
    ws.row_dimensions[row].height = 40
    ws.merge_cells(f'A{row}:E{row}')
    c = ws.cell(row=row, column=1, value=f'  {num}. {text}')
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='left', vertical='center', wrap_text=True, indent=1)
    row += 1

# ============================================================
# 6. 기타사항
# ============================================================
row += 1
row = section_title(ws, row, '6. 기타사항')

other_items = [
    ('6.1', '본 규격에 명시되지 않은 사항은 수요기관의 제시(협의가능)에 의하며 기타 제조사항은 KS규격 및 일반 제조요령에 따른다.'),
    ('6.2', '타인의 특허권·실용신안권·디자인권 등을 무단 사용할 경우 그로 인한 일체의 책임은 계약자에 있다.'),
]

for num, text in other_items:
    ws.row_dimensions[row].height = 36
    ws.merge_cells(f'A{row}:E{row}')
    c = ws.cell(row=row, column=1, value=f'  {num}. {text}')
    c.font = Font(name='맑은 고딕', size=10)
    c.alignment = Alignment(horizontal='left', vertical='center', wrap_text=True, indent=1)
    row += 1

# ============================================================
# Page setup
# ============================================================
ws.page_setup.orientation = 'portrait'
ws.page_setup.paperSize = 9  # A4
ws.page_margins.left = 0.7
ws.page_margins.right = 0.7
ws.page_margins.top = 0.75
ws.page_margins.bottom = 0.75

# ============================================================
# Save
# ============================================================
output_path = '/home/user/freescan_for_auto/3d_scan/물품규격서_핸드헬드3D스캔장비.xlsx'
wb.save(output_path)
print(f'Saved: {output_path}')
