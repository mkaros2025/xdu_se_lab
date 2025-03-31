// src/router/index.js

import { createRouter, createWebHistory } from 'vue-router'
import HomePage from '../views/HomePage.vue'
import LoginPage from '../views/LoginPage.vue'
import RegisterPage from '../views/RegisterPage.vue'
import MenuPage from '../views/MenuPage.vue'
import OrdersPage from '../views/OrdersPage.vue'
import OrderDetailView from '../views/OrderDetailView.vue'
import ReviewsPage from '../views/ReviewsPage.vue'
import NotFound from '../views/NotFound.vue'
import store from '../store' // 引入 Vuex store

const routes = [
  {
    path: '/',
    name: 'Home',
    component: HomePage,
  },
  {
    path: '/login',
    name: 'Login',
    component: LoginPage,
    meta: { guest: true },
  },
  {
    path: '/register',
    name: 'Register',
    component: RegisterPage,
    meta: { guest: true },
  },
  {
    path: '/menu',
    name: 'Menu',
    component: MenuPage,
    meta: { requiresAuth: true }, // 需要认证的路由
  },
  {
    path: '/orders',
    name: 'Orders',
    component: OrdersPage,
    meta: { requiresAuth: true },
  },
  {
    path: '/orders/:id',
    name: 'OrderDetail',
    component: OrderDetailView,
    meta: { requiresAuth: true },
    props: true,
  },
  {
    path: '/reviews',
    name: 'Reviews',
    component: ReviewsPage,
    meta: { requiresAuth: true },
  },
  {
    path: '/manage-menu',
    name: 'ManageMenu',
    component: () => import('../views/ManageMenu.vue'),
    meta: { requiresAuth: true, role: 'staff' }, // 仅 staff 角色可访问
  },
  {
    path: '/manage-orders',
    name: 'ManageOrders',
    component: () => import('../views/ManageOrders.vue'),
    meta: { requiresAuth: true, role: 'staff' },
  },
  {
    path: '/deliveries',
    name: 'Deliveries',
    component: () => import('../views/Deliveries.vue'),
    meta: { requiresAuth: true, role: 'delivery' },
  },
  {
    path: '/:pathMatch(.*)*',
    name: 'NotFound',
    component: NotFound,
  },
]

const router = createRouter({
  history: createWebHistory(),
  routes,
})

// 路由守卫
router.beforeEach((to, from, next) => {
  const requiresAuth = to.matched.some(record => record.meta.requiresAuth)
  const guestOnly = to.matched.some(record => record.meta.guest)
  const isAuthenticated = store.getters['auth/isAuthenticated']
  const user = store.getters['auth/getUser']

  if (requiresAuth && !isAuthenticated) {
    next({ name: 'Login' })
  } else if (guestOnly && isAuthenticated) {
    next({ name: 'Home' })
  } else if (requiresAuth && to.meta.role && user?.role !== to.meta.role) {
    next({ name: 'Home' }) // 或者显示“无权限”页面
  } else {
    next()
  }
})

export default router
