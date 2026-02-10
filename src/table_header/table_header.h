/**
 * File:   table_header.h
 * Author: AWTK Develop Team
 * Brief:  table_header
 *
 * Copyright (c) 2020 - 2025 Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2020-07-18 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_TABLE_HEADER_H
#define TK_TABLE_HEADER_H

#include "base/widget.h"

BEGIN_C_DECLS

/**
 * @enum table_header_event_type_t
 * @prefix EVT_TABLE_HEADER_
 * @annotation ["scriptable"]
 * table_header 事件类型。
 */
typedef enum _table_header_event_type_t {
  /**
   * @const EVT_TABLE_HEADER_CELL_DRAG_RESIZING
   * 单元格的尺寸正在被拖曳改变。
   */
  EVT_TABLE_HEADER_CELL_DRAG_RESIZING = EVT_USER_START + 1,

  /**
   * @const EVT_TABLE_HEADER_CELL_DRAG_RESIZED
   * 单元格的尺寸发生了改变。
   */
  EVT_TABLE_HEADER_CELL_DRAG_RESIZED
} table_header_event_type_t;

/**
 * @class table_header_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * table\_header。一个简单的容器控件，用来放置表头中的子控件。
 *
 * 它本身不提供布局功能，仅提供具有语义的标签，让xml更具有可读性。
 * 子控件的布局可用layout\_children属性指定。
 * 请参考[布局参数](https://github.com/zlgopen/awtk/blob/master/docs/layout.md)。
 *
 * table\_header\_t是[widget\_t](widget_t.md)的子类控件，widget\_t的函数均适用于table\_header\_t控件。
 *
 * 在xml中使用"table\_header"标签创建table\_header。如：
 *
 * ```xml
 * <!-- ui -->
 * <table_header x="0" y="0" w="200" h="30"/>
 * ```
 *
 * 可用通过style来设置控件的显示风格，如背景颜色等。如：
 *
 * ```xml
 * <!-- style -->
 * <table_header>
 *   <style name="default">
 *     <normal bg_color="#e0e0e0"/>
 *   </style>
 * </table_header>
 * ```
 */
typedef struct _table_header_t {
  widget_t widget;

  /**
   * @property {xy_t} offset
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 偏移量。
   */
  xy_t offset;

  /**
   * @property {wh_t} cell_min_size
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 最小列宽。
   */
  wh_t cell_min_size;

  /**
   * @property {bool_t} enable_cell_drag_resize
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否使能通过拖曳调整列宽。
   */
  bool_t enable_cell_drag_resize;

  /**
   * @property {char*} dragger_style
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 分割线的风格。
   */
  char* dragger_style;

  /* private */
  darray_t* cells;
  darray_t* draggers;
  xy_t save_offset;
  xy_t dragger_start_pos;
} table_header_t;

/**
 * @method table_header_create
 * @annotation ["constructor", "scriptable"]
 * 创建table_header对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} table_header对象。
 */
widget_t* table_header_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method table_header_cast
 * 转换为table_header对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget table_header对象。
 *
 * @return {widget_t*} table_header对象。
 */
widget_t* table_header_cast(widget_t* widget);

/**
 * @method table_header_get_virtual_size
 * 获取虚拟大小。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {int32_t} 返回虚拟大小。
 */
int32_t table_header_get_virtual_size(widget_t* widget);

/**
 * @method table_header_set_offset
 * 设置 偏移量。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {int32_t} yoffset 偏移量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t table_header_set_offset(widget_t* widget, int32_t offset);

/**
 * @method table_header_get_cell
 * 获取指定索引的单元格。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} index 索引位置。
 *
 * @return {widget_t*} 返回单元格对应的控件对象。
 */
widget_t* table_header_get_cell(widget_t* widget, uint32_t index);

/**
 * @method table_header_count_cells
 * 获取单元格数量。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {uint32_t} 返回单元格数量。
 */
uint32_t table_header_count_cells(widget_t* widget);

/**
 * @method table_header_restack_cell
 * 重新排列单元格的位置。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {widget_t*} cell 单元格控件。
 * @param {int32_t} index 索引位置号。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t table_header_restack_cell(widget_t* widget, widget_t* cell, int32_t index);

/**
 * @method table_header_set_enable_cell_drag_resize
 * 设置 是否使能通过拖曳调整列宽。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget vtree_header对象。
 * @param {bool_t} enable 是否开启。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t table_header_set_enable_cell_drag_resize(widget_t* widget, bool_t enable);

/**
 * @method table_header_set_dragger_style
 * 设置分割线的样式。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} style 样式名称。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t table_header_set_dragger_style(widget_t* widget, const char* style);

#define WIDGET_TYPE_TABLE_HEADER "table_header"

/**
 * @enum table_header_prop_t
 * @annotation ["scriptable", "string"]
 * @prefix TABLE_HEADER_PROP_
 * 控件的属性。
 */

/**
 * @const TABLE_HEADER_PROP_OFFSET
 * 偏移量。
 */
#define TABLE_HEADER_PROP_OFFSET "offset"

/**
 * @const TABLE_HEADER_PROP_CELL_MIN_SIZE
 * 单元格的最小尺寸。
 */
#define TABLE_HEADER_PROP_CELL_MIN_SIZE "cell_min_size"

/**
 * @const TABLE_HEADER_PROP_ENABLE_CELL_DRAG_RESIZE
 * 是否使能通过拖曳调整列宽。
 */
#define TABLE_HEADER_PROP_ENABLE_CELL_DRAG_RESIZE "enable_cell_drag_resize"

/**
 * @const TABLE_HEADER_PROP_DRAGGER_STYLE
 * 分割线的风格。
 */
#define TABLE_HEADER_PROP_DRAGGER_STYLE "dragger_style"

#define TABLE_HEADER(widget) ((table_header_t*)(table_header_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(table_header);

END_C_DECLS

#endif /*TK_TABLE_HEADER_H*/
