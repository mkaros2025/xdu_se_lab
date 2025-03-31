<template>
  <v-app-bar app color="primary" dark class="floating-navbar enhanced-navbar">
    <!-- 未登录时显示文本 -->
    <div v-if="!isAuthenticated" class="logo-container">
      <span class="logo-text">开吃！</span>
    </div>

    <!-- 已登录时显示当前用户信息 -->
    <v-menu v-if="isAuthenticated" transition="fab-transition" v-model="menu" offset-y>
      <template #activator="{ props }">
        <div
          class="ml-4 d-flex align-center"
          v-bind="props"
          style="cursor: pointer;"
        >
          <v-avatar size="32" class="avatar-hover">
            <v-icon>mdi-account</v-icon>
          </v-avatar>
          <span class="ml-2">{{ userDisplayName }}</span>
        </div>
      </template>

      <!-- 下拉菜单内容 -->
      <v-card class="user-info-card">
        <v-card-title class="headline">用户信息</v-card-title>
        <v-card-text>
          <v-list dense>
            <v-list-item>
              <v-list-item-content>
                <div><strong>用户名:</strong> {{ user.username || 'N/A' }}</div>
              </v-list-item-content>
            </v-list-item>
            <v-list-item>
              <v-list-item-content>
                <div><strong>姓名:</strong> {{ user.name || 'N/A' }}</div>
              </v-list-item-content>
            </v-list-item>
            <v-list-item>
              <v-list-item-content>
                <div><strong>身份:</strong> {{ user.role || 'N/A' }}</div>
              </v-list-item-content>
            </v-list-item>
            <v-list-item>
              <v-list-item-content>
                <div><strong>电话号码:</strong> {{ user.phone_number || 'N/A' }}</div>
              </v-list-item-content>
            </v-list-item>
            <v-list-item>
              <v-list-item-content>
                <div><strong>地址:</strong> {{ user.address || 'N/A' }}</div>
              </v-list-item-content>
            </v-list-item>
          </v-list>
        </v-card-text>
      </v-card>
    </v-menu>

    <v-spacer></v-spacer>
    <v-btn text to="/" class="navbar-btn">首页</v-btn>
    <v-btn text to="/menu" class="navbar-btn">菜单</v-btn>
    <v-btn text v-if="isCustomer" to="/orders" class="navbar-btn">我的订单</v-btn>
    <v-btn text v-if="isStaff" to="/manage-menu" class="navbar-btn">管理菜单</v-btn>
    <v-btn text v-if="isStaff" to="/manage-orders" class="navbar-btn">管理订单</v-btn>
    <v-btn text v-if="isDelivery" to="/deliveries" class="navbar-btn">配送管理</v-btn>
    <v-btn text v-if="!isAuthenticated" to="/login" class="navbar-btn">登录</v-btn>
    <v-btn text v-if="!isAuthenticated" to="/register" class="navbar-btn">注册</v-btn>
    <v-btn text v-if="isAuthenticated" @click="handleLogout" class="navbar-btn">退出</v-btn>
  </v-app-bar>
</template>

<script>
import { ref, computed, watch } from 'vue'
import { useStore } from 'vuex'
import { useRouter } from 'vue-router'

export default {
  name: 'AppNavbar',
  setup() {
    const store = useStore()
    const router = useRouter()
    const menu = ref(false)

    // 获取 Vuex getters
    const isAuthenticated = computed(() => store.getters['auth/isAuthenticated'])
    const user = computed(() => store.getters['auth/getUser'])

    // 监视 user 对象以调试
    watch(user, (newUser) => {
      console.log('当前用户对象:', newUser)
    })

    // 计算属性根据用户角色
    const isCustomer = computed(() => {
      return isAuthenticated.value && user.value && user.value.role === 'customer'
    })
    const isStaff = computed(() => {
      return isAuthenticated.value && user.value && user.value.role === 'staff'
    })
    const isDelivery = computed(() => {
      return isAuthenticated.value && user.value && user.value.role === 'delivery'
    })

    // 显示名称
    const userDisplayName = computed(() => {
      return user.value?.username || user.value?.name || '用户'
    })

    // 方法: 退出并关闭菜单
    const handleLogout = () => {
      store.dispatch('auth/logout')
    }

    return {
      menu,
      isAuthenticated,
      isCustomer,
      isStaff,
      isDelivery,
      userDisplayName,
      user,
      handleLogout,
    }
  },
}
</script>

<style scoped>
.ml-4 {
  margin-left: 1rem;
}
.ml-2 {
  margin-left: 0.5rem;
}
.d-flex {
  display: flex;
  align-items: center;
}

/* 炫酷的导航栏样式 */
.floating-navbar {
  margin-top: 3px;
  border-radius: 8px;
  padding-left: 10px;
  padding-right: 10px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
  background: linear-gradient(to right, rgba(63, 81, 181, 0.8), rgba(33, 150, 243, 0.8));
  backdrop-filter: blur(8px);
}

/* logo 样式 */
.logo-container {
  display: flex;
  align-items: center;
}

.logo-text {
  font-size: 1.8rem;
  font-weight: bold;
  color: #FFFFFF; /* 修改为白色 */
  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.4);
}

/* 用户头像 hover 效果 */
.avatar-hover {
  transition: transform 0.3s, box-shadow 0.3s;
}
.avatar-hover:hover {
  transform: scale(1.2);
  box-shadow: 0 4px 12px rgba(255, 255, 255, 0.5); /* 修改为白色阴影 */
}

/* 按钮样式改进 */
.navbar-btn {
  color: white;
  font-weight: bold;
  text-transform: uppercase;
  transition: color 0.3s, text-shadow 0.3s;
}
.navbar-btn:hover {
  color: #FFFFFF; /* 修改为白色 */
  text-shadow: 0 0 10px rgba(255, 255, 255, 0.8); /* 修改为白色阴影 */
}
</style>
