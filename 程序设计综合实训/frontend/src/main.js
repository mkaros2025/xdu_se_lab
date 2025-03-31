// src/main.js

import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import vuetify from './plugins/vuetify' // 引入 Vuetify 配置
import '@mdi/font/css/materialdesignicons.css' // 引入 MDI 图标
import './plugins/webfontloader' // 引入字体加载（如果有）
import apiClient from './services/api' // 引入 API 服务

const app = createApp(App)

app.use(router)
app.use(store)
app.use(vuetify)
app.config.globalProperties.$api = apiClient

app.mount('#app')

// 在应用启动时，检查是否有 userId
const userId = localStorage.getItem('userId')
if (userId) {
  store.dispatch('auth/fetchUser').catch(() => {
    store.dispatch('auth/logout')
  })
}
