# table_view 控件 FAQ

## 如何修改 table_view 指定行的数据

可以通过遍历 table_client 的子控件，来获取每一项 table_row，并通过 table_row 中的 index 属性来获取当前行号，获取到想要的 table_row 后，再通过 widget_lookup 接口查找想要修改的控件的数据。

> 由于 table_view 控件的特性，所有的 table_row 并不会被全部创建出来，创建出来的 table_row 数量大约为可视范围的 3 倍，在查找时需要注意这一点。