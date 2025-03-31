<!-- src/views/HomePage.vue -->

<template>
  <v-container>
    <!-- 欢迎卡片 -->
    <v-row justify="center">
      <v-col cols="12" md="8">
        <v-card class="pa-5 text-center" elevation="6">
          <v-card-title>
            <v-icon large color="primary">mdi-food</v-icon>
          </v-card-title>
          <v-card-text>
            <v-typo variant="display-2" class="custom-title primary--text mb-2">
              欢迎使用 kkk餐厅订餐系统
            </v-typo>
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
    
    <!-- 简化后的当前用户信息 -->
    <v-row justify="center" v-if="isAuthenticated && user" class="mt-4">
      <v-col cols="12" md="8" class="text-center">
        <v-typo variant="h1" class="primary--text">
          你好，{{ displayRole }} {{ user.username }}
        </v-typo>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
import { mapGetters } from 'vuex'

export default {
  name: 'HomePage',
  computed: {
    ...mapGetters('auth', {
      isAuthenticated: 'isAuthenticated',
      user: 'getUser'
    }),
    // 计算属性，用于将用户角色转换为中文
    displayRole() {
      const roleMap = {
        customer: '顾客',
        staff: '员工',
        delivery: '配送员'
      }
      return roleMap[this.user.role] || this.user.role
    }
  },
}
</script>

<style scoped>
.mt-4 {
  margin-top: 1.5rem;
}

.mb-2 {
  margin-bottom: 0.5rem;
}

.custom-title {
  font-size: 2rem; /* 调整主标题字体大小 */
}

.custom-subtitle {
  font-size: 1.5rem; /* 调整副标题字体大小 */
}

.v-typo.h4 {
  font-weight: bold;
}
</style>
