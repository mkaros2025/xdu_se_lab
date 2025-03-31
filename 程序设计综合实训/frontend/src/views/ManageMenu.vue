<!-- src/views/ManageMenu.vue -->
<template>
  <v-container>
    <h1>菜单管理</h1>
    <v-btn color="primary" @click="openAddDialog" class="mb-4">添加菜品</v-btn>

    <!-- 菜单列表 -->
    <v-data-table
      :headers="headers"
      :items="menuItems"
      class="elevation-1"
      :loading="loading"
      loading-text="加载中..."
      item-key="item_id"
    >
      <!-- 自定义图片列的渲染 -->
      <template v-slot:item.image_url="{ item }">
        <v-img
          :src="item.image_url || defaultImage"
          max-width="80"
          contain
          class="rounded-lg my-2"
        ></v-img>
      </template>

      <!-- 自定义操作列的渲染 -->
      <template v-slot:item.actions="{ item }">
        <div class="d-flex justify-end">
          <v-btn icon @click="openEditDialog(item)">
            <v-icon>mdi-pencil</v-icon>
          </v-btn>
          <v-btn icon @click="deleteItem(item.item_id)">
            <v-icon color="red">mdi-delete</v-icon>
          </v-btn>
        </div>
      </template>

      <!-- 自定义表头的对齐（可选） -->
      <template v-slot:header.image_url>
        <span class="text-center">图片</span>
      </template>
      <template v-slot:header.name>
        <span class="text-start">名称</span>
      </template>
      <template v-slot:header.description>
        <span class="text-start">描述</span>
      </template>
      <template v-slot:header.price>
        <span class="text-end">价格</span>
      </template>
      <template v-slot:header.actions>
        <span class="text-center">操作</span>
      </template>
    </v-data-table>

    <!-- 添加/编辑菜品对话框 -->
    <v-dialog v-model="dialog" max-width="500px">
      <v-card>
        <v-card-title>
          <span class="headline">{{ dialogTitle }}</span>
        </v-card-title>
        <v-card-text>
          <v-form ref="form" v-model="valid">
            <v-text-field
              v-model="formData.name"
              label="菜品名称"
              :rules="[v => !!v || '菜品名称为必填项']"
              required
            ></v-text-field>

            <v-textarea
              v-model="formData.description"
              label="描述"
            ></v-textarea>

            <v-text-field
              v-model="formData.price"
              label="价格"
              type="number"
              :rules="[v => !!v || '价格为必填项', v => v > 0 || '价格必须大于0']"
              required
            ></v-text-field>

            <v-text-field
              v-model="formData.image_url"
              label="图片 URL"
              type="url"
            ></v-text-field>
          </v-form>
          <!-- 图片预览 -->
          <v-img
            :src="formData.image_url || defaultImage"
            height="200px"
            class="mt-2 rounded-lg"
            contain
          ></v-img>
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text @click="closeDialog">取消</v-btn>
          <v-btn color="primary" :disabled="!valid" @click="submitForm">
            保存
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>

    <!-- 删除确认对话框 -->
    <v-dialog v-model="deleteDialog" max-width="500px">
      <v-card>
        <v-card-title class="headline">确认删除</v-card-title>
        <v-card-text>
          确定要删除菜品 "{{ itemToDelete?.name }}" 吗？
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text @click="closeDeleteDialog">取消</v-btn>
          <v-btn color="red" @click="confirmDelete">删除</v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>

    <!-- 成功与错误消息 -->
    <v-snackbar v-model="snackbar.show" :color="snackbar.color" timeout="6000">
      {{ snackbar.message }}
      <v-btn color="white" text @click="snackbar.show = false">关闭</v-btn>
    </v-snackbar>
  </v-container>
</template>

<script>
import { mapGetters, mapActions } from 'vuex'

export default {
  name: 'ManageMenu',
  data() {
    return {
      headers: [
        { text: '图片', value: 'image_url', align: 'center' },
        { text: '名称', value: 'name', align: 'start' },
        { text: '描述', value: 'description', align: 'start' },
        { text: '价格', value: 'price', align: 'end' },
        { text: '操作', value: 'actions', sortable: false, align: 'center' },
      ],
      dialog: false,
      dialogTitle: '',
      formData: {
        item_id: null,
        name: '',
        description: '',
        price: '',
        image_url: '',
      },
      valid: false,
      deleteDialog: false,
      itemToDelete: null,
      defaultImage: 'https://via.placeholder.com/200', // 默认图片
      loading: false,
      snackbar: {
        show: false,
        message: '',
        color: '',
      },
    }
  },
  computed: {
    ...mapGetters('menu', ['getMenuItems']),
    menuItems() {
      return this.getMenuItems
    },
  },
  methods: {
    ...mapActions('menu', ['fetchMenu', 'addMenuItem', 'updateMenuItem', 'deleteMenuItem']),
    openAddDialog() {
      this.dialogTitle = '添加菜品'
      this.formData = {
        item_id: null,
        name: '',
        description: '',
        price: '',
        is_available: 1,
        image_url: '',
      }
      this.dialog = true
    },
    openEditDialog(item) {
      this.dialogTitle = '编辑菜品'
      this.formData = { ...item }
      this.dialog = true
    },
    closeDialog() {
      this.dialog = false
    },
    async submitForm() {
      if (this.$refs.form.validate()) {
        try {
          if (this.formData.item_id) {
            // 更新菜品
            await this.updateMenuItem(this.formData)
            this.snackbar.message = '菜品更新成功！'
            this.snackbar.color = 'success'
          } else {
            // 添加菜品
            await this.addMenuItem(this.formData)
            this.snackbar.message = '菜品添加成功！'
            this.snackbar.color = 'success'
          }
          this.snackbar.show = true
          this.dialog = false
          this.fetchMenu()
        } catch (error) {
          // 更详细的错误信息
          const errorMessage =
            error.response && error.response.data && error.response.data.message
              ? error.response.data.message
              : '操作失败，请重试。'
          this.snackbar.message = errorMessage
          this.snackbar.color = 'error'
          this.snackbar.show = true
          console.error('提交表单失败：', error)
        }
      }
    },
    deleteItem(itemId) {
      this.itemToDelete = this.menuItems.find(item => item.item_id === itemId)
      this.deleteDialog = true
    },
    closeDeleteDialog() {
      this.deleteDialog = false
      this.itemToDelete = null
    },
    async confirmDelete() {
      try {
        await this.deleteMenuItem(this.itemToDelete.item_id)
        this.snackbar.message = '菜品删除成功！'
        this.snackbar.color = 'success'
        this.snackbar.show = true
        this.fetchMenu()
      } catch (error) {
        const errorMessage =
          error.response && error.response.data && error.response.data.message
            ? error.response.data.message
            : '删除失败，请重试。'
        this.snackbar.message = errorMessage
        this.snackbar.color = 'error'
        this.snackbar.show = true
        console.error('删除菜品失败：', error)
      } finally {
        this.deleteDialog = false
        this.itemToDelete = null
      }
    },
  },
  created() {
    this.fetchMenu()
  },
}
</script>

<style scoped>
/* 可根据需要添加样式 */
.mb-4 {
  margin-bottom: 1rem;
}
.mt-2 {
  margin-top: 0.5rem;
}
.rounded-lg {
  border-radius: 12px;
}
.d-flex {
  display: flex;
}
.justify-end {
  justify-content: flex-end;
}
.text-center {
  text-align: center;
}
.text-start {
  text-align: left;
}
.text-end {
  text-align: right;
}
.headline {
  font-weight: bold;
}
</style>
