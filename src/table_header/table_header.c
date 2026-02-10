/**
 * File:   table_header.c
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

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "table_header.h"
#include "base/widget_vtable.h"
#include "base/layout.h"
#include "base/window_manager.h"
#include "widgets/dragger.h"

#define TABLE_HEADER_DRAGGER_SIZE 4
#define TABLE_HEADER_DEFAULT_CELL_MIN_SIZE 30
#define TABLE_HEADER_CELL_SAVE_SIZE_WHEN_DRAG " saved_size "
#define TABLE_HEADER_CELL_SAVE_POS_WHEN_DRAG " saved_pos "

#define TABLE_HEADER_FOR_EACH_CELL_BEGIN(header, iter, i)    \
  if (header->cells != NULL && header->cells->size > 0) {    \
    int32_t i = 0;                                           \
    int32_t nr = header->cells->size;                        \
    widget_t** children = (widget_t**)(header->cells->elms); \
    for (i = 0; i < nr; i++) {                               \
      widget_t* iter = children[i];

#define TABLE_HEADER_FOR_EACH_CELL_BEGIN_R(header, iter, i)  \
  if (header->cells != NULL && header->cells->size > 0) {    \
    int32_t i = 0;                                           \
    int32_t nr = header->cells->size;                        \
    widget_t** children = (widget_t**)(header->cells->elms); \
    for (i = nr - 1; i >= 0; i--) {                          \
      widget_t* iter = children[i];

#define TABLE_HEADER_FOR_EACH_CELL_END() \
  }                                      \
  }

#define TABLE_HEADER_FOR_EACH_DRAGGER_BEGIN(header, iter, i)    \
  if (header->draggers != NULL && header->draggers->size > 0) { \
    int32_t i = 0;                                              \
    int32_t nr = header->draggers->size;                        \
    widget_t** children = (widget_t**)(header->draggers->elms); \
    for (i = 0; i < nr; i++) {                                  \
      widget_t* iter = children[i];

#define TABLE_HEADER_FOR_EACH_DRAGGER_BEGIN_R(header, iter, i)  \
  if (header->draggers != NULL && header->draggers->size > 0) { \
    int32_t i = 0;                                              \
    int32_t nr = header->draggers->size;                        \
    widget_t** children = (widget_t**)(header->draggers->elms); \
    for (i = nr - 1; i >= 0; i--) {                             \
      widget_t* iter = children[i];

#define TABLE_HEADER_FOR_EACH_DRAGGER_END() \
  }                                         \
  }

static bool_t widget_is_dragger(widget_t* widget) {
  return widget != NULL && tk_str_eq(widget_get_type(widget), WIDGET_TYPE_DRAGGER);
}

ret_t table_header_set_offset(widget_t* widget, int32_t offset) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, RET_BAD_PARAMS);

  if (header->offset != offset) {
    header->offset = offset;
    widget_invalidate_force(widget, NULL);
  }

  return RET_OK;
}

static ret_t table_header_set_cell_min_size(widget_t* widget, wh_t size) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, RET_BAD_PARAMS);

  if (header->cell_min_size != size) {
    header->cell_min_size = size;
    widget_invalidate_force(widget, NULL);
  }

  return RET_OK;
}

ret_t table_header_set_enable_cell_drag_resize(widget_t* widget, bool_t enable) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, RET_BAD_PARAMS);

  if (header->enable_cell_drag_resize != enable) {
    header->enable_cell_drag_resize = enable;
  }

  return RET_OK;
}

ret_t table_header_set_dragger_style(widget_t* widget, const char* style) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL && style != NULL, RET_BAD_PARAMS);

  if (!tk_str_eq(header->dragger_style, style)) {
    header->dragger_style = tk_str_copy(header->dragger_style, style);

    if (header->draggers != NULL) {
      for (int32_t i = 0; i < header->draggers->size; ++i) {
        widget_t* dragger = WIDGET(darray_get(header->draggers, i));
        widget_use_style(dragger, style);
      }
    }
  }

  return RET_OK;
}

static ret_t table_header_on_paint_children_horizontal(widget_t* widget, canvas_t* c) {
  rect_t r_save;
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  table_header_t* header = TABLE_HEADER(widget);
  rect_t r = rect_init(c->ox, c->oy, widget->w, widget->h);
  int32_t xoffset = -header->offset;

  canvas_translate(c, xoffset, 0);
  canvas_get_clip_rect(c, &r_save);

  r = rect_intersect(&r, &r_save);

  if (vg != NULL) {
    vgcanvas_save(vg);
    vgcanvas_clip_rect(vg, (float_t)r.x, (float_t)r.y, (float_t)r.w, (float_t)r.h);
  }

  canvas_set_clip_rect(c, &r);
  widget_on_paint_children_default(widget, c);
  canvas_set_clip_rect(c, &r_save);
  canvas_untranslate(c, xoffset, 0);

  if (vg != NULL) {
    vgcanvas_clip_rect(vg, (float_t)r_save.x, (float_t)r_save.y, (float_t)r_save.w,
                       (float_t)r_save.h);
    vgcanvas_restore(vg);
  }
  return RET_OK;
}

static ret_t table_header_get_offset(widget_t* widget, xy_t* out_x, xy_t* out_y) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL && out_x != NULL && out_y != NULL, RET_BAD_PARAMS);
  *out_x = header->offset;
  *out_y = 0;

  return RET_OK;
}

static ret_t table_header_on_enter_dragger(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(e->target);
  widget_set_cursor(widget, WIDGET_CURSOR_SIZEWE);
  return RET_OK;
}

static ret_t table_header_on_leave_dragger(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(e->target);
  widget_set_cursor(widget, WIDGET_CURSOR_DEFAULT);
  return RET_OK;
}

static ret_t table_header_on_move_dragger(void* ctx, event_t* e) {
  return RET_STOP;
}

static ret_t table_header_visit_save_cell_size_horizontal(void* ctx, const void* iter) {
  widget_t* widget = (WIDGET(iter));
  widget_set_prop_int(widget, TABLE_HEADER_CELL_SAVE_SIZE_WHEN_DRAG, widget->w);
  widget_set_prop_int(widget, TABLE_HEADER_CELL_SAVE_POS_WHEN_DRAG, widget->x);

  return RET_OK;
}

static ret_t table_header_visit_save_dragger_pos_horizontal(void* ctx, const void* iter) {
  widget_t* widget = (WIDGET(iter));
  widget_set_prop_int(widget, TABLE_HEADER_CELL_SAVE_POS_WHEN_DRAG, widget->x);

  return RET_OK;
}

static ret_t table_header_on_cell_drag_start_horizontal(void* ctx, event_t* e) {
  xy_t min_x = 0;
  xy_t max_x = 9999;
  widget_t* widget = WIDGET(e->target);
  widget_t* parent = widget->parent;
  table_header_t* header = TABLE_HEADER(parent);
  int32_t index = darray_find_index(header->draggers, widget);

  if (index > -1) {
    widget_t* col = darray_get(header->cells, index);
    min_x = col->x + header->cell_min_size;
  }

  widget_set_prop_int(widget, "x_min", min_x);
  widget_set_prop_int(widget, "x_max", max_x);

  header->save_offset = header->offset;
  header->dragger_start_pos = widget->x;

  darray_foreach(header->cells, table_header_visit_save_cell_size_horizontal, NULL);
  darray_foreach(header->draggers, table_header_visit_save_dragger_pos_horizontal, NULL);

  return RET_OK;
}

static int32_t table_header_index_of_dragger(widget_t* widget) {
  table_header_t* header = TABLE_HEADER(widget->parent);
  int32_t index = darray_find_index(header->draggers, widget);

  return index;
}

static ret_t table_header_on_cell_dragging_horizontal(void* ctx, event_t* e) {
  widget_t* iter = NULL;
  widget_t* widget = WIDGET(e->target);
  widget_t* parent = widget->parent;
  table_header_t* header = TABLE_HEADER(parent);
  int32_t size = header->draggers->size;
  int32_t delta = 0;
  int32_t i = 0;
  int32_t cell_size = 0;
  int32_t save_w = 0;
  int32_t save_x = 0;
  int32_t index = table_header_index_of_dragger(widget);
  point_t header_point = {parent->x, parent->y};

  widget_to_screen(parent, &header_point);
  delta = widget->x - header->dragger_start_pos;
  iter = table_header_get_cell(parent, index);
  save_w = widget_get_prop_int(iter, TABLE_HEADER_CELL_SAVE_SIZE_WHEN_DRAG, iter->w);
  if (delta > 0) {
    widget_move_resize(iter, iter->x, iter->y, save_w + delta, iter->h);
  } else {
    cell_size = tk_max(save_w + delta, header->cell_min_size);
    widget_move_resize(iter, iter->x, iter->y, cell_size, iter->h);
    header->offset = tk_max(header->save_offset + delta, 0);
  }

  widget_layout(widget);
  widget_dispatch_simple_event(parent, EVT_TABLE_HEADER_CELL_DRAG_RESIZING);
  return RET_OK;
}

static ret_t table_header_on_cell_drag_end_horizontal(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(e->target);
  widget_set_cursor(widget, WIDGET_CURSOR_DEFAULT);

  widget_dispatch_simple_event(widget, EVT_TABLE_HEADER_CELL_DRAG_RESIZED);

  return RET_OK;
}

static ret_t table_header_init_dragger(widget_t* widget, widget_t* dragger) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL && dragger != NULL, RET_BAD_PARAMS);

  if (header->dragger_style != NULL) {
    widget_use_style(dragger, header->dragger_style);
  }
  widget_restack(dragger, widget_count_children(widget) - 1);

  widget_on(dragger, EVT_POINTER_ENTER, table_header_on_enter_dragger, NULL);
  widget_on(dragger, EVT_POINTER_LEAVE, table_header_on_leave_dragger, NULL);
  widget_on(dragger, EVT_POINTER_MOVE, table_header_on_move_dragger, NULL);
  widget_on(dragger, EVT_DRAG_START, table_header_on_cell_drag_start_horizontal, NULL);
  widget_on(dragger, EVT_DRAG, table_header_on_cell_dragging_horizontal, NULL);
  widget_on(dragger, EVT_DRAG_END, table_header_on_cell_drag_end_horizontal, NULL);

  return RET_OK;
}

static ret_t table_header_on_add_child(widget_t* widget, widget_t* child) {
  table_header_t* header = TABLE_HEADER(widget);

  if (header->enable_cell_drag_resize && !widget_is_dragger(child)) {
    if (header->cells == NULL) {
      header->cells = darray_create(4, NULL, NULL);
    }
    assert(header->cells != NULL);
    darray_push(header->cells, child);
    if (widget_add_child_default(widget, child) == RET_OK) {
      if (header->cells->size > 0) {
        widget_t* dragger = dragger_create(widget, 0, 0, 0, 0);
        if (header->draggers == NULL) {
          header->draggers = darray_create(4, NULL, NULL);
        }
        assert(header->draggers != NULL && dragger != NULL);
        table_header_init_dragger(widget, dragger);
        darray_push(header->draggers, dragger);
      }
    }
    return RET_OK;
  } else {
    return widget_add_child_default(widget, child);
  }
}

static ret_t table_header_on_remove_child(widget_t* widget, widget_t* child) {
  table_header_t* header = TABLE_HEADER(widget);
  darray_t* cells = header->cells;
  darray_t* draggers = header->draggers;

  if (!widget->destroying && !widget_is_dragger(child)) {
    if (cells != NULL) {
      int32_t index = darray_find_index(cells, child);
      if (index > -1) {
        if (draggers != NULL && index < draggers->size) {
          widget_t* dragger = WIDGET(darray_get(draggers, index));
          darray_remove_index(draggers, index);
          if (dragger != NULL) {
            widget_destroy(dragger);
          }
        }
        darray_remove_index(cells, index);
      }
    }
  }

  return RET_CONTINUE;
}

static ret_t table_header_restack_dragger(widget_t* widget) {
  table_header_t* header = TABLE_HEADER(widget);
  darray_t* draggers = header->draggers;

  if (draggers != NULL && draggers->size > 0) {
    uint32_t count = widget_count_children(widget);
    TABLE_HEADER_FOR_EACH_DRAGGER_BEGIN(header, iter, i)
    widget_restack(iter, count - 1);
    TABLE_HEADER_FOR_EACH_DRAGGER_END()
  }

  return RET_OK;
}

static ret_t table_header_on_layout_children(widget_t* widget) {
  table_header_t* header = TABLE_HEADER(widget);

  if (header->enable_cell_drag_resize) {
    int32_t x = 0;

    widget_layout_children_default(widget);

    TABLE_HEADER_FOR_EACH_CELL_BEGIN(header, iter, i)
    widget_move_resize(iter, x, iter->y, tk_max(iter->w, header->cell_min_size), widget->h);
    x += iter->w;
    widget_layout(iter);
    TABLE_HEADER_FOR_EACH_CELL_END()

    table_header_restack_dragger(widget);

    TABLE_HEADER_FOR_EACH_DRAGGER_BEGIN(header, iter, i)
    widget_t* col = darray_get(header->cells, i);
    xy_t x = tk_max(col->x + col->w - TABLE_HEADER_DRAGGER_SIZE / 2, 0);
    widget_move_resize(iter, x, iter->y, TABLE_HEADER_DRAGGER_SIZE, widget->h);
    TABLE_HEADER_FOR_EACH_DRAGGER_END()

    return RET_OK;
  } else {
    return widget_layout_children_default(widget);
  }
}

int32_t table_header_get_virtual_size(widget_t* widget) {
  int32_t size = 0;
  darray_t* cells;
  darray_t* draggers;
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, 0);

  cells = header->cells;
  if (cells != NULL) {
    for (int32_t i = 0; i < cells->size; i++) {
      widget_t* iter = WIDGET(darray_get(cells, i));
      assert(iter != NULL);
      size += iter->w;
    }
  }

  draggers = header->draggers;
  if (draggers != NULL) {
    widget_t* dragger = WIDGET(darray_get(draggers, 0));
    assert(dragger != NULL);
    size += dragger->w / 2 * (draggers->size + 1);
  }

  return tk_max(size, widget->w);
}

static ret_t table_header_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    return table_header_set_offset(widget, value_int(v));
  } else if (tk_str_eq(name, TABLE_HEADER_PROP_CELL_MIN_SIZE)) {
    return table_header_set_cell_min_size(widget, value_int(v));
  } else if (tk_str_eq(name, TABLE_HEADER_PROP_ENABLE_CELL_DRAG_RESIZE)) {
    return table_header_set_enable_cell_drag_resize(widget, value_bool(v));
  } else if (tk_str_eq(name, TABLE_HEADER_PROP_DRAGGER_STYLE)) {
    const char* style = value_str(v);
    return table_header_set_dragger_style(widget, style);
  }

  return RET_NOT_FOUND;
}

static ret_t table_header_get_prop(widget_t* widget, const char* name, value_t* v) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    value_set_int(v, header->offset);
    return RET_OK;
  } else if (tk_str_eq(name, TABLE_HEADER_PROP_CELL_MIN_SIZE)) {
    value_set_int(v, header->cell_min_size);
    return RET_OK;
  } else if (tk_str_eq(name, TABLE_HEADER_PROP_ENABLE_CELL_DRAG_RESIZE)) {
    value_set_bool(v, header->enable_cell_drag_resize);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t table_header_on_destroy(widget_t* widget) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, RET_BAD_PARAMS);

  if (header->cells != NULL) {
    darray_destroy(header->cells);
    header->cells = NULL;
  }

  if (header->draggers != NULL) {
    darray_destroy(header->draggers);
    header->draggers = NULL;
  }

  TKMEM_FREE(header->dragger_style);

  return RET_OK;
}

const char* s_table_header_properties[] = {
    TABLE_HEADER_PROP_OFFSET, TABLE_HEADER_PROP_CELL_MIN_SIZE,
    TABLE_HEADER_PROP_ENABLE_CELL_DRAG_RESIZE, TABLE_HEADER_PROP_DRAGGER_STYLE, NULL};

TK_DECL_VTABLE(table_header) = {
    .type = WIDGET_TYPE_TABLE_HEADER,
    .size = sizeof(table_header_t),
    .parent = TK_PARENT_VTABLE(widget),
    .clone_properties = s_table_header_properties,
    .persistent_properties = s_table_header_properties,
    .create = table_header_create,
    .set_prop = table_header_set_prop,
    .get_prop = table_header_get_prop,
    .get_offset = table_header_get_offset,
    .on_add_child = table_header_on_add_child,
    .on_remove_child = table_header_on_remove_child,
    .on_layout_children = table_header_on_layout_children,
    .on_paint_children = table_header_on_paint_children_horizontal,
    .on_destroy = table_header_on_destroy,
};

widget_t* table_header_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(table_header), x, y, w, h);
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, NULL);

  header->offset = 0;
  header->enable_cell_drag_resize = FALSE;
  header->cell_min_size = TABLE_HEADER_DEFAULT_CELL_MIN_SIZE;

  return widget;
}

widget_t* table_header_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, table_header), NULL);

  return widget;
}

widget_t* table_header_get_cell(widget_t* widget, uint32_t index) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, NULL);

  if (header->cells != NULL && index < header->cells->size) {
    return WIDGET(darray_get(header->cells, index));
  }
  return NULL;
}

uint32_t table_header_count_cells(widget_t* widget) {
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL, 0);

  if (header->cells != NULL) {
    return header->cells->size;
  }
  return 0;
}

ret_t table_header_restack_cell(widget_t* widget, widget_t* cell, int32_t index) {
  int32_t raw;
  table_header_t* header = TABLE_HEADER(widget);
  return_value_if_fail(header != NULL && header->cells != NULL && cell != NULL, RET_BAD_PARAMS);

  raw = darray_find_index(header->cells, cell);
  if (raw > -1) {
    darray_remove_index(header->cells, raw);
    darray_insert(header->cells, index, cell);
  }

  return RET_OK;
}
