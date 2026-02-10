/**
 * File:   table_row.c
 * Author: AWTK Develop Team
 * Brief:  table_row
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
 * 2020-07-17 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "table_row.h"
#include "base/widget_vtable.h"
#include "base/layout.h"
#include "../table_header/table_header.h"

ret_t table_row_set_index(widget_t* widget, uint32_t index) {
  table_row_t* table_row = TABLE_ROW(widget);
  return_value_if_fail(table_row != NULL, RET_BAD_PARAMS);

  table_row->index = index;

  return RET_OK;
}

static ret_t table_row_get_prop(widget_t* widget, const char* name, value_t* v) {
  table_row_t* table_row = TABLE_ROW(widget);
  return_value_if_fail(table_row != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(TABLE_ROW_PROP_INDEX, name)) {
    value_set_uint32(v, table_row->index);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t table_row_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(TABLE_ROW_PROP_INDEX, name)) {
    table_row_set_index(widget, value_uint32(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t table_row_on_event(widget_t* widget, event_t* e) {
  ret_t ret = RET_OK;
  uint16_t type = e->type;
  table_row_t* table_row = TABLE_ROW(widget);
  return_value_if_fail(table_row != NULL && widget != NULL, RET_BAD_PARAMS);

  switch (type) {
    case EVT_POINTER_DOWN: {
      pointer_event_t* evt = pointer_event_cast(e);
      table_row->down.x = evt->x;
      table_row->down.y = evt->y;
      break;
    }
    case EVT_POINTER_UP: {
      pointer_event_t* evt = pointer_event_cast(e);
      int dx = tk_abs(table_row->down.x - evt->x);
      int dy = tk_abs(table_row->down.y - evt->y);

      if (dx < 3 && dy < 3) {
        pointer_event_t click;
        pointer_event_init(&click, EVT_CLICK, widget, evt->x, evt->y);
        widget_dispatch(widget, (event_t*)(&click));
      }
      break;
    }
    default:
      break;
  }

  return RET_OK;
}

static widget_t* table_row_try_find_table_header(widget_t* widget) {
  widget_t* client = widget->parent;
  widget_t* view = client->parent;
  widget_t* header = NULL;
  if (view != NULL) {
    header = widget_lookup_by_type(view, WIDGET_TYPE_TABLE_HEADER, FALSE);
  }

  return header;
}

static ret_t table_row_on_layout_children(widget_t* widget) {
  table_row_t* row = TABLE_ROW(widget);
  table_header_t* header = TABLE_HEADER(table_row_try_find_table_header(widget));

  if (header != NULL && header->enable_cell_drag_resize) {
    widget_t* cell = NULL;

    WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, j)
    cell = table_header_get_cell(WIDGET(header), j);
    if (cell != NULL) {
      widget_move_resize(iter, cell->x, cell->y, cell->w, widget->h);
      widget_layout(iter);
    }
    WIDGET_FOR_EACH_CHILD_END()
    return RET_OK;
  } else {
    return widget_layout_children_default(widget);
  }
}

const char* s_table_row_properties[] = {TABLE_ROW_PROP_INDEX, NULL};

TK_DECL_VTABLE(table_row) = {.size = sizeof(table_row_t),
                             .type = WIDGET_TYPE_TABLE_ROW,
                             .clone_properties = s_table_row_properties,
                             .persistent_properties = s_table_row_properties,
                             .parent = TK_PARENT_VTABLE(widget),
                             .create = table_row_create,
                             .on_event = table_row_on_event,
                             .set_prop = table_row_set_prop,
                             .get_prop = table_row_get_prop,
                             .on_layout_children = table_row_on_layout_children};

widget_t* table_row_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(table_row), x, y, w, h);
  table_row_t* table_row = TABLE_ROW(widget);
  return_value_if_fail(table_row != NULL, NULL);

  return widget;
}

widget_t* table_row_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, table_row), NULL);

  return widget;
}
