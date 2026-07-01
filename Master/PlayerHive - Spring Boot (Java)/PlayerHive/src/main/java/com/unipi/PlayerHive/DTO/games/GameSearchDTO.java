package com.unipi.PlayerHive.DTO.games;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import org.springframework.data.annotation.Id;

@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter

public class GameSearchDTO {

    @Id
    private String id;
    private String name;

    private Double price;
    private Integer discount;
    private Double finalPrice;

    private String imageURL;
}
