<!-- src/views/MenuPage.vue -->

<template>
  <v-container fluid class="no-padding">
    <h1 class="text-center my-8">又到饭点了😋</h1>
    <v-row justify="start" gap="6">
      <v-col
        v-for="item in menuItems"
        :key="item.item_id"
        cols="12"
        sm="6"
        md="3"
        class="d-flex justify-center"
      >
        <v-card class="hoverable small-card" elevation="4">
          <!-- 图片部分 -->
          <div class="image-wrapper">
            <v-img
              :src="item.image_url || defaultImage"
              height="160px"
              class="rounded-top"
              cover
            ></v-img>
          </div>

          <!-- 卡片内容 -->
          <v-card-title class="d-flex justify-space-between align-center">
            <span class="headline">{{ item.name }}</span>
            <span class="price">¥{{ item.price }}</span>
          </v-card-title>
          <v-card-text class="description">
            {{ item.description }}
          </v-card-text>

          <!-- 购物车操作 -->
          <v-card-actions v-if="isCustomer">
            <v-row class="w-100" align="center">
              <v-col cols="5">
                <v-text-field
                  v-model.number="quantities[item.item_id]"
                  label="数量"
                  type="number"
                  min="1"
                  dense
                  hide-details
                  @change="validateQuantity(item.item_id)"
                ></v-text-field>
              </v-col>
              <v-col cols="7" class="d-flex justify-end">
                <v-btn
                  color="primary"
                  dark
                  @click="addItemToCart(item)"
                  small
                >
                  添加至购物车
                </v-btn>
              </v-col>
            </v-row>
          </v-card-actions>
        </v-card>
      </v-col>
    </v-row>

    <!-- 错误信息 -->
    <v-alert v-if="error" type="error" dismissible class="mt-4">
      {{ errorMessage }}
    </v-alert>

    <!-- 成功消息 -->
    <v-snackbar v-model="snackbar.show" :color="snackbar.color" timeout="3000">
      {{ snackbar.message }}
      <v-btn color="white" text @click="snackbar.show = false">关闭</v-btn>
    </v-snackbar>
  </v-container>
</template>

<script>
import { ref, reactive, computed, watch, onMounted } from 'vue'
import { useStore } from 'vuex'

export default {
  name: 'MenuPage',
  setup() {
    const store = useStore()
    const isAuthenticated = computed(() => store.getters['auth/isAuthenticated'])
    const user = computed(() => store.getters['auth/getUser'])

    const isCustomer = computed(() => {
      return isAuthenticated.value && user.value && user.value.role === 'customer'
    })
    const isStaff = computed(() => {
      return isAuthenticated.value && user.value.role === 'staff'
    })
    const isDelivery = computed(() => {
      return isAuthenticated.value && user.value.role === 'delivery'
    })

    // 定义响应式数据
    const defaultImage = 'https://via.placeholder.com/400x160' // 默认图片
    const error = ref(false)
    const errorMessage = ref('')
    const quantities = reactive({}) // 用于存储每个菜品的数量
    const snackbar = reactive({
      show: false,
      message: '',
      color: '',
    })

    // 计算属性: 获取菜单项
    const getMenuItems = computed(() => store.getters['menu/getMenuItems'])

    // 调试日志
    const menuItems = computed(() => {
      console.log('菜单项目：', getMenuItems.value) // 调试日志
      return getMenuItems.value
    })

    // 异步方法: 获取菜单项并初始化数量
    const fetchMenuItems = async () => {
      try {
        await store.dispatch('menu/fetchMenu')
        // 初始化 quantities 对象
        menuItems.value.forEach(item => {
          // 确保响应式添加属性
          if (!(item.item_id in quantities)) {
            quantities[item.item_id] = 1
            console.log(`初始化 quantities[${item.item_id}] = 1`)
          }
        })
        console.log('初始化 quantities 完成')
      } catch (err) {
        error.value = true
        errorMessage.value = '获取菜单失败，请稍后再试。'
        console.error('获取菜单失败：', err)
      }
    }

    // 方法: 验证数量是否有效
    const validateQuantity = (itemId) => {
      if (quantities[itemId] < 1) {
        quantities[itemId] = 1
      }
    }

    // 方法: 将菜品添加到购物车
    const addItemToCart = (item) => {
      const quantity = quantities[item.item_id]
      const itemToAdd = {
        ...item,
        quantity,
      }
      store.dispatch('orders/addToCart', itemToAdd)
      snackbar.message = `${item.name} 已添加到购物车！`
      snackbar.color = 'success'
      snackbar.show = true
      // 重置数量为1
      quantities[item.item_id] = 1
    }

    // 生命周期钩子: 组件挂载时获取菜单项
    onMounted(() => {
      fetchMenuItems()
    })

    // 监视 menuItems 的变化，确保 quantities 对象同步
    watch(menuItems, (newMenuItems) => {
      newMenuItems.forEach(item => {
        if (!(item.item_id in quantities)) {
          quantities[item.item_id] = 1
          console.log(`监视初始化 quantities[${item.item_id}] = 1`)
        }
      })
    })

    return {
      defaultImage,
      error,
      errorMessage,
      quantities,
      snackbar,
      menuItems,
      validateQuantity,
      addItemToCart,
      isCustomer,
      isStaff,
      isDelivery,
      isAuthenticated,
    }
  },
}
</script>

<style scoped>
/* 移除容器的左右内边距 */
.no-padding {
  padding-left: 100px; /* 根据需要调整 */
  padding-right: 100px; /* 根据需要调整 */
}

/* 缩小卡片的最大宽度 */
.small-card {
  max-width: 600px; /* 从280px增大到320px */
}

/* 包裹图片的容器，应用圆角和溢出隐藏 */
.image-wrapper {
  border-top-left-radius: 12px;
  border-top-right-radius: 12px;
  overflow: hidden;
}

/* 应用卡片顶部圆角 */
.rounded-top {
  border-top-left-radius: 12px !important;
  border-top-right-radius: 12px !important;
}

/* 增加卡片悬停效果 */
.hoverable {
  transition: transform 0.3s, box-shadow 0.3s;
}
.hoverable:hover {
  transform: translateY(-5px);
  box-shadow: 0 8px 16px rgba(0, 0, 0, 0.2);
}

/* 调整卡片标题和价格的样式 */
.headline {
  font-size: 1.2rem; /* 调整标题字体大小 */
  font-weight: bold;
  margin-bottom: 0.2rem;
}

.price {
  font-size: 1.2rem; /* 调整价格字体大小，使其与标题一致 */
  color: #4caf50; /* 使用绿色突出价格 */
  font-weight: bold;
}

/* 调整描述的样式，使其颜色更浅 */
.description {
  font-size: 0.9rem; /* 调整描述字体大小 */
  color: #9e9e9e; /* 使用更浅的灰色 */
  line-height: 1.4;
}

/* 优化按钮对齐和间距 */
.v-card-actions {
  padding: 12px 16px; /* 缩小内边距 */
}

/* 优化数量输入框 */
.v-text-field {
  max-width: 150px; /* 限制输入框宽度 */
}

/* 其他样式优化 */
.text-center {
  text-align: center;
}

.my-8 {
  margin-top: 2rem;
  margin-bottom: 2rem;
}

/* 响应式调整 */
@media (max-width: 600px) {
  .small-card {
    max-width: 100%; /* 在小屏幕上卡片宽度为100% */
  }
}
</style>
