/**
 * File:   table_view.c
 * Author: AWTK Develop Team
 * Brief:  表格视图。
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
 * 2020-07-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "table_view.h"
#include "scroll_view/scroll_bar.h"
#include "../table_client/table_client.h"
#include "../table_header/table_header.h"

static ret_t table_view_on_client_scrolled(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  table_view_t* table_view = TABLE_VIEW(widget);
  table_client_t* client = TABLE_CLIENT(table_view->client);
  table_header_t* header = TABLE_HEADER(table_view->header);
  widget_t* vbar = table_view->vbar;
  widget_t* hbar = table_view->hbar;

  if (vbar != NULL && client != NULL) {
    int64_t value = client->yoffset;
    int64_t row_height = client->row_height;
    int64_t virtual_h = table_client_get_virtual_h(WIDGET(client));

    scroll_bar_set_params(vbar, tk_max(vbar->h, virtual_h), row_height);

    if (virtual_h <= vbar->h) {
      value = 0;
    } else {
      value = value * virtual_h / (virtual_h - vbar->h);
    }

    scroll_bar_set_value_only(vbar, (int32_t)value);
  }

  if (hbar != NULL && header != NULL) {
    int64_t value = header->offset;
    int64_t virtual_size = table_header_get_virtual_size(WIDGET(header));

    scroll_bar_set_params(hbar, tk_max(hbar->w, virtual_size), 1);

    if (virtual_size <= hbar->w) {
      value = 0;
    } else {
      value = value * virtual_size / (virtual_size - hbar->w);
    }

    scroll_bar_set_value_only(hbar, (int32_t)value);
  }

  return RET_OK;
}

static ret_t table_view_on_scroll_bar_changed(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  table_view_t* table_view = TABLE_VIEW(widget);
  scroll_bar_t* bar = SCROLL_BAR(e->target);
  widget_t* header = table_view->header;
  widget_t* client = table_view->client;

  if (tk_str_eq(WIDGET(bar)->name, "hbar")) {
    int64_t value = bar->value;
    int64_t virtual_w = bar->virtual_size;
    if (virtual_w > 0) {
      double scale = (virtual_w - widget->w) / (double)virtual_w;
      int32_t offset = value * scale;
      if (header != NULL) {
        table_header_set_offset(header, offset);
      }
      if (client != NULL) {
        table_client_set_xoffset(client, offset);
      }
    }
  } else if (tk_str_eq(WIDGET(bar)->name, "vbar")) {
    if (client != NULL) {
      int64_t value = bar->value;
      int64_t virtual_h = bar->virtual_size;
      if (virtual_h > 0) {
        double scale = (virtual_h - client->h) / (double)virtual_h;
        emitter_disable(client->emitter);
        table_client_set_yoffset(client, value * scale);
        emitter_enable(client->emitter);
      }
    }
  }

  return RET_OK;
}

static ret_t table_view_on_scroll_bar_resized(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  table_view_t* table_view = TABLE_VIEW(widget);
  scroll_bar_t* bar = SCROLL_BAR(e->target);
  widget_t* header = table_view->header;
  widget_t* client = table_view->client;

  if (tk_str_eq(WIDGET(bar)->name, "hbar")) {
    int64_t value = bar->value;
    int64_t virtual_w = bar->virtual_size;
    if (virtual_w > 0) {
      double scale = (virtual_w - widget->w) / (double)virtual_w;
      int32_t offset = value * scale;
      if (header != NULL) {
        table_header_set_offset(header, offset);
      }
      if (client != NULL) {
        table_client_set_xoffset(client, offset);
      }
    }
  } else if (tk_str_eq(WIDGET(bar)->name, "vbar")) {
    widget_t* client = table_view->client;
    if (client != NULL) {
      int64_t value = bar->value;
      int64_t virtual_h = bar->virtual_size;
      if (virtual_h > 0) {
        double scale = (virtual_h - client->h) / (double)virtual_h;
        table_client_set_yoffset_ex(client, value * scale, TRUE);
      }
    }
  }

  return RET_OK;
}

static ret_t table_view_on_header_col_changing(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  table_view_t* table_view = TABLE_VIEW(widget);
  widget_t* header = table_view->header;
  widget_t* client = table_view->client;
  widget_t* hbar = table_view->hbar;

  if (hbar != NULL) {
    int64_t value = TABLE_HEADER(header)->offset;
    int32_t virtual_size = table_header_get_virtual_size(header);
    scroll_bar_set_params(hbar, tk_max(hbar->w, virtual_size), 1);
    if (virtual_size <= hbar->w) {
      value = 0;
    } else {
      value = value * virtual_size / (virtual_size - hbar->w);
    }
    scroll_bar_set_value_only(hbar, (int32_t)value);
  }

  table_client_set_xoffset(client, TABLE_HEADER(header)->offset);
  table_client_relayout_row(client, header);
  return RET_OK;
}

ret_t table_view_on_add_child(widget_t* widget, widget_t* child) {
  table_view_t* table_view = TABLE_VIEW(widget);
  const char* type = widget_get_type(child);

  if (tk_str_eq(type, WIDGET_TYPE_SCROLL_BAR_MOBILE) ||
      tk_str_eq(type, WIDGET_TYPE_SCROLL_BAR_DESKTOP)) {
    widget_on(child, EVT_RESIZE, table_view_on_scroll_bar_resized, widget);
    widget_on(child, EVT_MOVE_RESIZE, table_view_on_scroll_bar_resized, widget);
    widget_on(child, EVT_VALUE_CHANGING, table_view_on_scroll_bar_changed, widget);
    widget_on(child, EVT_VALUE_CHANGED, table_view_on_scroll_bar_changed, widget);
    table_view->vbar = child;
  } else if (tk_str_eq(type, WIDGET_TYPE_TABLE_CLIENT)) {
    table_view->client = child;
    widget_on(child, EVT_SCROLL, table_view_on_client_scrolled, widget);
  } else if (tk_str_eq(type, WIDGET_TYPE_TABLE_HEADER)) {
    table_view->header = child;
    widget_on(child, EVT_TABLE_HEADER_CELL_DRAG_RESIZING, table_view_on_header_col_changing, widget);
    widget_on(child, EVT_TABLE_HEADER_CELL_DRAG_RESIZED, table_view_on_header_col_changing, widget);

  }

  return RET_FAIL;
}

static ret_t table_view_on_event(widget_t* widget, event_t* e) {
  ret_t ret = RET_OK;
  table_view_t* table_view = TABLE_VIEW(widget);
  return_value_if_fail(widget != NULL && table_view != NULL, RET_BAD_PARAMS);

  switch (e->type) {
    case EVT_WIDGET_LOAD: {
      /**
       * 等待子控件名称初始化完成再设置相应的滚动条
       */
      table_view->vbar = widget_child(widget, "vbar");
      table_view->hbar = widget_child(widget, "hbar");
      break;
    }
    default:
      break;
  }

  return ret;
}

ret_t table_view_set_virtual_width(widget_t* widget, int32_t virtual_size) {
  widget_t* hbar = widget_lookup(widget, "hbar", FALSE);
  table_view_t* table_view = TABLE_VIEW(widget);
  table_client_t* client = TABLE_CLIENT(table_view->client);
  if (hbar && client) {
    int64_t value = client->xoffset;
    scroll_bar_set_params(hbar, tk_max(hbar->w, virtual_size), 1);
    if (virtual_size <= hbar->w) {
      value = 0;
    } else {
      value = value * virtual_size / (virtual_size - hbar->w);
    }

    scroll_bar_set_value_only(hbar, (int32_t)value);
  }

  return RET_OK;
}

const char* s_table_view_properties[] = {NULL};

TK_DECL_VTABLE(table_view) = {
    .size = sizeof(table_view_t),
    .type = WIDGET_TYPE_TABLE_VIEW,
    .clone_properties = s_table_view_properties,
    .persistent_properties = s_table_view_properties,
    .parent = TK_PARENT_VTABLE(widget),
    .create = table_view_create,
    .on_add_child = table_view_on_add_child,
    .on_event = table_view_on_event,
};

widget_t* table_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(table_view), x, y, w, h);
  table_view_t* table_view = TABLE_VIEW(widget);
  return_value_if_fail(table_view != NULL, NULL);

  return widget;
}

widget_t* table_view_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, table_view), NULL);

  return widget;
}
