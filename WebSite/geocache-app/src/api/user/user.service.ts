import { Injectable } from '@nestjs/common';
import { User } from './user.entity';
import { InjectRepository } from '@nestjs/typeorm';
import { Repository } from 'typeorm';

@Injectable()
export class UserService {
    constructor(
        @InjectRepository(User)
        private usersRepository: Repository<User>
    ) {

    }

    async findAll() {
        return await this.usersRepository.find(
            { loadRelationIds: true}
        );
    }

    async save( user: { idUser: number, name: string, email: string, flag:boolean, password:string, admin:boolean}) {
        return await this.usersRepository.save(user);
    }

    async remove(idUser: string) {
        return await this.usersRepository.delete(idUser);
    }
}
